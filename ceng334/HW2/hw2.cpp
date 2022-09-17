#include <pthread.h>
#include <iostream>
#include <vector>
#include <utility>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <functional>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/sem.h>
//#include "sem.h"
extern "C"
{
#include "hw2_output.h"
}

using namespace std;

pthread_mutex_t mutexx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
vector<vector<pthread_cond_t>> canGather; //= PTHREAD_COND_INITIALIZER;
vector<vector<pthread_cond_t>> canSmoke;  // = PTHREAD_COND_INITIALIZER;
pthread_cond_t order = PTHREAD_COND_INITIALIZER;
pthread_cond_t wait = PTHREAD_COND_INITIALIZER;
pthread_cond_t wait_smoke = PTHREAD_COND_INITIALIZER;

vector<vector<int>> smokecount;
vector<vector<int>> gathercount;
vector<vector<int>> smokewait;
vector<vector<int>> gatherwait;

vector<vector<sem_t>> gridSem;

void* gather(void* arg);
void* smoke(void* arg);
void* timer(void* arg);


vector<vector<int>> grid;
vector<vector<vector<int>>> privates;
vector<pair<int, string>> orders;
vector<vector<vector<int>>> smokers;

int BREAK, CONTINUE, STOP;

int main()
{
	hw2_init_notifier();
	BREAK = CONTINUE = STOP = 0;
	// TAKE INPUT
	int col, row, privNum, ordNum, smokeNum;
	cin >> row >> col;
	vector<int> tmp;
	int val;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{

			cin >> val;
			tmp.push_back(val);
		}
		grid.push_back(tmp);
		tmp.clear();
	}

	cin >> privNum;
	vector<vector<int>> priv;
	for (int i = 0; i < privNum; i++)
	{
		priv.clear();
		tmp.clear();
		for (int j = 0; j < 5; j++)
		{
			int tmp0;
			cin >> tmp0;
			tmp.push_back(tmp0);
		}
		priv.push_back(tmp);
		for (int j = 0; j < tmp[4]; j++)
		{
			vector<int> tmp1;
			int x, y;
			cin >> x >> y;
			tmp1.push_back(x);
			tmp1.push_back(y);
			priv.push_back(tmp1);
		}
		privates.push_back(priv);
	}
	if (cin >> ordNum) {
		for (int i = 0; i < ordNum; i++)
		{
			int tmp;
			string tmp1;
			cin >> tmp >> tmp1;
			orders.push_back(make_pair(tmp, tmp1));
		}

		if (cin >> smokeNum) {
			for (int i = 0; i < smokeNum; i++)
			{
				priv.clear();
				tmp.clear();
				for (int j = 0; j < 3; j++)
				{
					int tmp0;
					cin >> tmp0;
					tmp.push_back(tmp0);
				}
				priv.push_back(tmp);
				for (int j = 0; j < tmp[2]; j++)
				{
					vector<int> tmp1;
					int x, y, num;
					cin >> x >> y >> num;
					tmp1.push_back(x);
					tmp1.push_back(y);
					tmp1.push_back(num);
					priv.push_back(tmp1);
				}
				smokers.push_back(priv);
			}
		}
		else {
			smokeNum = 0;
		}
	}
	else {
		ordNum = smokeNum = 0;
	}

	/*

		////////////////////////// FOR DEBUG //////////////////////////

		//GRID PRINT
		cout << col << row << endl;
		cout << "printing grid:" << endl;
		for (int i = 0;i < row;i++) {
			for (int j = 0;j < col;j++) {
				cout << grid[i][j] << " ";
			}
			cout << endl;
		}
		//DONE

		//PRIVATE PRINT
		cout << "Printing privates:" << endl;
		for (int i = 0;i < privNum;i++) {
			cout << privates[i][0][0] << " " << privates[i][0][1] << " " << privates[i][0][2] << " " << privates[i][0][3] << endl;
		}
		//DONE

		//ORD PRINT
		cout << "Printing orders: " << endl;
		for (int i = 0;i < ordNum;i++) {
			cout << orders[i].first << ", " << orders[i].second << endl;
		}
		//DONEint *arg = malloc(sizeof(*arg));
	" << endl;
		for (int i = 0;i < smokeNum;i++) {
			cout << smokers[i][0][0] << " " << smokers[i][0][1] << " " << smokers[i][0][2] << " " << endl;
		}
		//DONE

	*/

	//////////////////////////////////////////////////////////////////////////////////////

	// DONE
	// grid[row][col]
	// privates[privNum][0][privInfo] // Private  parameters (id, constant_i, constant_j, tg, ng)
	// privates[privNum][1..*][areas to clear]
	// smokers[privNum][0][privInfo] // Smoker  parameters (id, time to smoke, cell count)
	// smokers[privNum][1..*][areas to litter+how many cigs]

	gridSem.resize(row);
	canGather.resize(row);
	canSmoke.resize(row);
	smokecount.resize(row);
	gathercount.resize(row);
	smokewait.resize(row);
	gatherwait.resize(row);
	for (int i = 0; i < row; i++)
	{
		gridSem[i].resize(col);
		canGather[i].resize(col);
		canSmoke[i].resize(col);
		smokecount[i].resize(col);
		gathercount[i].resize(col);
		smokewait[i].resize(col);
		gatherwait[i].resize(col);
		for (int j = 0; j < col; j++)
		{
			sem_init(&(gridSem[i][j]), 0, 1);
			canGather[i][j] = PTHREAD_COND_INITIALIZER;
			canSmoke[i][j] = PTHREAD_COND_INITIALIZER;
			smokecount[i][j] = 0;
			gathercount[i][j] = 0;
			smokewait[i][j] = 0;
			gatherwait[i][j] = 0;
		}
	}

	//cerr << "main" << endl;
	pthread_t tid[privates.size()];
	for (int i = 0; i < privates.size(); i++)
	{
		// PRIVATES THREADS
		int* index = (int*)malloc(sizeof(*index));

		if (!index)
		{
			fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
			exit(EXIT_FAILURE);
		}

		*index = i;

		pthread_create(&tid[i], NULL, gather, index);
	}

	pthread_t tid2[smokers.size()];
	for (int i = 0; i < smokers.size(); i++)
	{
		// SMOKERS THREADS
		int* index = (int*)malloc(sizeof(*index));

		if (!index)
		{
			fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
			exit(EXIT_FAILURE);
		}

		*index = i;

		pthread_create(&tid2[i], NULL, smoke, index);
	}

	// MAIN THREAD USED TO EXECUTE ORDERS 
	sort(orders.begin(), orders.end());
	int prev = 0;

	for (int ind = 0;ind < orders.size();ind++) {

		if (ind == 0)
			usleep(orders[ind].first * 1000);
		else
			usleep((orders[ind].first - prev) * 1000);

		prev = orders[ind].first;

		pthread_mutex_lock(&mutexx);

		if (orders[ind].second == "break") {

			BREAK = 1;
			hw2_notify(ORDER_BREAK, 0, 0, 0);

			if (!BREAK) {
				continue;
			}

			pthread_cond_broadcast(&wait);

			for (int i = 0;i < row;i++) {
				for (int j = 0;j < col;j++) {
					pthread_cond_broadcast(&canGather[i][j]);
					pthread_cond_signal(&canSmoke[i][j]);
					gathercount[i][j] = 0;
				}
			}
		}

		if (orders[ind].second == "continue") {

			BREAK = 0;
			hw2_notify(ORDER_CONTINUE, 0, 0, 0);

			if (BREAK) {
				continue;
			}

			pthread_cond_broadcast(&order);

		}

		if (orders[ind].second == "stop") {

			STOP = 1;
			hw2_notify(ORDER_STOP, 0, 0, 0);

			pthread_cond_broadcast(&wait);
			pthread_cond_broadcast(&wait_smoke);
			pthread_cond_broadcast(&order);

			for (int i = 0;i < row;i++) {
				for (int j = 0;j < col;j++) {
					pthread_cond_broadcast(&canGather[i][j]);
					pthread_cond_broadcast(&canSmoke[i][j]);
					for (int z = 0;z < smokers.size();z++) {
						sem_post(&gridSem[i][j]);
					}
				}
			}
		}

		pthread_mutex_unlock(&mutexx);

	}

	// JOIN THREADS
	for (int i = 0; i < smokers.size(); i++)
	{
		pthread_join(tid2[i], NULL);
	}

	for (int i = 0; i < privates.size(); i++)
	{
		pthread_join(tid[i], NULL);
	}

	// DESTROY SYNC MECHANISMS
	pthread_mutex_destroy(&mutexx);
	pthread_mutex_destroy(&tmp_mutex);

	pthread_cond_destroy(&order);
	pthread_cond_destroy(&wait);
	pthread_cond_destroy(&wait_smoke);


	for (int i = 0;i < row;i++) {
		for (int j = 0;j < col;j++) {
			sem_destroy(&gridSem[i][j]);
			pthread_cond_destroy(&canGather[i][j]);
			pthread_cond_destroy(&canSmoke[i][j]);
		}
	}

	return 1;
}

void* gather(void* arg)
{
	int n = *((int*)arg);
	hw2_notify(PROPER_PRIVATE_CREATED, privates[n][0][0], 0, 0); // Notify immediately

	// privates[privNum][0][privInfo] // Private  parameters (id, constant_i, constant_j, tg, ng)
	// privates[privNum][1..*][areas to clear]
	// INIT VARIABLES
	int gid, si, sj, tg, ng;
	vector<pair<int, int>> areas;
	gid = privates[n][0][0];
	si = privates[n][0][1];
	sj = privates[n][0][2];
	tg = privates[n][0][3];
	ng = privates[n][0][4];
	for (int i = 0; i < ng; i++)
	{
		areas.push_back(make_pair(privates[n][1 + i][0], privates[n][1 + i][1]));
	}
	////////////////////////////////////////////////

	struct timeval tval;
	struct timespec tspec;

	int tRow, tCol;
	int ind = 0;
	while (ind < ng)
	{
		pthread_mutex_lock(&mutexx);
		if (BREAK) {
			hw2_notify(PROPER_PRIVATE_TOOK_BREAK, gid, 0, 0);
			pthread_cond_wait(&order, &mutexx);
			if (STOP) {
				hw2_notify(PROPER_PRIVATE_STOPPED, gid, 0, 0);
				free(arg);
				pthread_mutex_unlock(&mutexx);
				pthread_exit(NULL);
			}
		}
		pthread_mutex_unlock(&mutexx);

		pthread_mutex_lock(&mutexx);
		tRow = areas[ind].first;
		tCol = areas[ind].second;
		for (int i = tRow; i < tRow + si; i++)
		{
			for (int j = tCol; j < tCol + sj; j++)
			{
				// LOCK AREAS
				if (gathercount[i][j] || smokecount[i][j])
				{
					++gatherwait[i][j];
					pthread_cond_wait(&canGather[i][j], &mutexx);
					--gatherwait[i][j];
				}
				if (BREAK || STOP) {
					break;
				}
				gathercount[i][j] = 1;
			}
			if (BREAK || STOP) {
				break;
			}
		}
		pthread_mutex_unlock(&mutexx);

		if (STOP) {
			hw2_notify(PROPER_PRIVATE_STOPPED, gid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}
		if (BREAK) {
			continue;
		}
		hw2_notify(PROPER_PRIVATE_ARRIVED, gid, tRow, tCol);

		for (int i = tRow; i < tRow + si; i++)
		{
			for (int j = tCol; j < tCol + sj; j++)
			{
				while (grid[i][j])
				{
					// GATHER CIGGIES

					gettimeofday(&tval, NULL);
					tspec.tv_sec = time(NULL) + tg / 1000;
					tspec.tv_nsec = tval.tv_usec * 1000 + 1000 * 1000 * (tg % 1000);
					tspec.tv_sec += tspec.tv_nsec / (1000 * 1000 * 1000);
					tspec.tv_nsec %= (1000 * 1000 * 1000);
					pthread_mutex_lock(&tmp_mutex);
					int val = pthread_cond_timedwait(&wait, &tmp_mutex, &tspec);
					// SIMILAR TO USLEEP

					if (val == 0 || BREAK || STOP) {
						pthread_mutex_unlock(&tmp_mutex);
						break;
					}

					pthread_mutex_unlock(&tmp_mutex);

					grid[i][j]--;

					hw2_notify(PROPER_PRIVATE_GATHERED, gid, i, j);
				}
				if (BREAK || STOP) {
					break;
				}

			}
			if (BREAK || STOP) {
				break;
			}

		}
		if (STOP) {
			hw2_notify(PROPER_PRIVATE_STOPPED, gid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}
		if (BREAK) {
			continue;
		}
		hw2_notify(PROPER_PRIVATE_CLEARED, gid, 0, 0);
		pthread_mutex_lock(&mutexx);
		for (int i = tRow; i < tRow + si; i++)
		{
			for (int j = tCol; j < tCol + sj; j++)
			{
				// UNLOCK AREAS
				if (BREAK || STOP) {
					break;
				}
				gathercount[i][j] = 0;
				if (smokewait[i][j])
					pthread_cond_signal(&canSmoke[i][j]);
				else
					pthread_cond_signal(&canGather[i][j]);
			}
			if (BREAK || STOP) {
				break;
			}
		}
		pthread_mutex_unlock(&mutexx);
		if (STOP) {
			hw2_notify(PROPER_PRIVATE_STOPPED, gid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}
		if (BREAK) {
			continue;
		}
		ind++;
	}
	hw2_notify(PROPER_PRIVATE_EXITED, gid, 0, 0);
	free(arg);
	pthread_exit(NULL);
}

void* smoke(void* arg)
{
	int n = *((int*)arg);
	hw2_notify(SNEAKY_SMOKER_CREATED, smokers[n][0][0], 0, 0);

	// smokers[privNum][0][privInfo] // Smoker  parameters (id, time to smoke, cell count)
	// smokers[privNum][1..*][areas to litter+how many cigs]
	// INIT VARIABLES
	int sid, ts, ns;
	vector<vector<int>> areas;
	sid = smokers[n][0][0];
	ts = smokers[n][0][1];
	ns = smokers[n][0][2];
	vector<int> tmp;

	for (int i = 0; i < ns; i++)
	{
		tmp.clear();
		tmp.push_back(smokers[n][1 + i][0]);
		tmp.push_back(smokers[n][1 + i][1]);
		tmp.push_back(smokers[n][1 + i][2]);
		areas.push_back(tmp);
	}
	////////////////////////////////////////////////

	struct timeval tval;
	struct timespec tspec;

	int tRow, tCol, tSmoke, i, j;
	for (int ind = 0; ind < ns; ind++)
	{

		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}

		tRow = areas[ind][0];
		tCol = areas[ind][1];
		tSmoke = areas[ind][2];

		sem_wait(&gridSem[tRow][tCol]);
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}

		///////////////////////// LOCK AREAS /////////////////////////
		pthread_mutex_lock(&mutexx);
		//
		i = tRow - 1;
		j = tCol - 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow - 1;
		j = tCol;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow - 1;
		j = tCol + 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow;
		j = tCol + 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow + 1;
		j = tCol + 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow + 1;
		j = tCol;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow + 1;
		j = tCol - 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow;
		j = tCol - 1;
		if (gathercount[i][j] || gatherwait[i][j])
		{
			smokewait[i][j]++;
			pthread_cond_wait(&canSmoke[i][j], &mutexx);
			smokewait[i][j]--;
		}
		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			pthread_mutex_unlock(&mutexx);
			free(arg);
			pthread_exit(NULL);
		}
		//
		i = tRow - 1;
		j = tCol - 1;
		smokecount[i][j]++;

		i = tRow - 1;
		j = tCol;
		smokecount[i][j]++;

		i = tRow - 1;
		j = tCol + 1;
		smokecount[i][j]++;

		i = tRow;
		j = tCol + 1;
		smokecount[i][j]++;

		i = tRow + 1;
		j = tCol + 1;
		smokecount[i][j]++;

		i = tRow + 1;
		j = tCol;
		smokecount[i][j]++;

		i = tRow + 1;
		j = tCol - 1;
		smokecount[i][j]++;

		i = tRow;
		j = tCol - 1;
		smokecount[i][j]++;

		//

		i = tRow - 1;
		j = tCol - 1;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow - 1;
		j = tCol;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow - 1;
		j = tCol + 1;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow;
		j = tCol + 1;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow + 1;
		j = tCol + 1;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow + 1;
		j = tCol;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow + 1;
		j = tCol - 1;
		pthread_cond_signal(&canSmoke[i][j]);

		i = tRow;
		j = tCol - 1;
		pthread_cond_signal(&canSmoke[i][j]);

		//

		pthread_mutex_unlock(&mutexx);

		///////////////////////////////////////////////////////////////////////////

		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}
		hw2_notify(SNEAKY_SMOKER_ARRIVED, sid, tRow, tCol);

		///////////////////////// SMOKE /////////////////////////
		int count = 0;
		while (count != tSmoke)
		{
			gettimeofday(&tval, NULL);
			tspec.tv_sec = time(NULL) + ts / 1000;
			tspec.tv_nsec = tval.tv_usec * 1000 + 1000 * 1000 * (ts % 1000);
			tspec.tv_sec += tspec.tv_nsec / (1000 * 1000 * 1000);
			tspec.tv_nsec %= (1000 * 1000 * 1000);
			pthread_mutex_lock(&tmp_mutex);
			int val = pthread_cond_timedwait(&wait_smoke, &tmp_mutex, &tspec);
			if (val == 0 || STOP) {
				pthread_mutex_unlock(&tmp_mutex);
				hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
				free(arg);
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&tmp_mutex);


			if (count % 8 == 0)
			{
				i = tRow - 1;
				j = tCol - 1;
			}
			if (count % 8 == 1)
			{
				i = tRow - 1;
				j = tCol;
			}
			if (count % 8 == 2)
			{
				i = tRow - 1;
				j = tCol + 1;
			}
			if (count % 8 == 3)
			{
				i = tRow;
				j = tCol + 1;
			}
			if (count % 8 == 4)
			{
				i = tRow + 1;
				j = tCol + 1;
			}
			if (count % 8 == 5)
			{
				i = tRow + 1;
				j = tCol;
			}
			if (count % 8 == 6)
			{
				i = tRow + 1;
				j = tCol - 1;
			}
			if (count % 8 == 7)
			{
				i = tRow;
				j = tCol - 1;
			}
			if (STOP) {
				hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
				free(arg);
				pthread_exit(NULL);
			}
			grid[i][j]++;
			count++;
			hw2_notify(SNEAKY_SMOKER_FLICKED, sid, i, j);
		}
		///////////////////////////////////////////////////////////////////////////

		if (STOP) {
			hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
			free(arg);
			pthread_exit(NULL);
		}
		hw2_notify(SNEAKY_SMOKER_LEFT, sid, 0, 0);

		///////////////////////// UNLOCK AREAS /////////////////////////
		i = tRow - 1;
		j = tCol - 1;
		pthread_mutex_lock(&mutexx);
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow - 1;
		j = tCol;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow - 1;
		j = tCol + 1;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow;
		j = tCol + 1;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow + 1;
		j = tCol + 1;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow + 1;
		j = tCol;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow + 1;
		j = tCol - 1;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);

		i = tRow;
		j = tCol - 1;
		smokecount[i][j]--;
		if (smokecount[i][j] == 0)
			pthread_cond_signal(&canGather[i][j]);
		pthread_mutex_unlock(&mutexx);

		///////////////////////////////////////////////////////////////////////////

		sem_post(&gridSem[tRow][tCol]);

	}

	hw2_notify(SNEAKY_SMOKER_EXITED, sid, 0, 0);
	free(arg);
	pthread_exit(NULL);
}
