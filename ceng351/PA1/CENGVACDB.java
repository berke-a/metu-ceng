package ceng.ceng351.cengvacdb;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

public class CENGVACDB implements ICENGVACDB {

    private static final String user = "e2468502";
    private static final String password = "EagUrJDW!dlm";
    private static final String host = "144.122.71.121";
    private static final String database = "db2468502";
    private static final int port = 8080;

    private static Connection connection = null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database + "?autoReconnect=true&useSSL=false";

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection = DriverManager.getConnection(url, user, password);
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        int num = 0;

        String queryCreateUserTable = "create table User (" +
                "userID int not null," +
                "userName varchar(30)," +
                "age int," +
                "address varchar(150)," +
                "password varchar(30)," +
                "status varchar(15)," +
                "primary key (userID))";
        String queryCreateVaccineTable = "create table Vaccine (" +
                "code int not null," +
                "vaccinename varchar(30)," +
                "type varchar(30)," +
                "primary key (code))";
        String queryCreateVaccinationTable = "create table Vaccination (" +
                "code int not null," +
                "userID int not null," +
                "dose int not null," +
                "vacdate date," +
                "primary key (dose,code,userID)," +
                "foreign key (code) references Vaccine(code) on delete cascade," +
                "foreign key (userID) references User(userID) on delete cascade)";
        String queryCreateAllergicSideEffectTable = "create table AllergicSideEffect (" +
                "effectcode int not null," +
                "effectname varchar(50)," +
                "primary key (effectcode))";
        String queryCreateSeenTable = "create table Seen (" +
                "effectcode int," +
                "code int," +
                "userID int," +
                "date date," +
                "degree varchar(30)," +
                "primary key (effectcode,code,userID),"+
                "foreign key (effectcode) references AllergicSideEffect(effectcode) on delete cascade," +
                "foreign key (code) references Vaccine(code) on delete cascade," +
                "foreign key (userID) references User(userID) on delete cascade)";

        try {

            Statement statement = connection.createStatement();

            statement.executeUpdate(queryCreateUserTable);
            num++;

            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {

            Statement statement = connection.createStatement();

            statement.executeUpdate(queryCreateVaccineTable);
            num++;

            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {

            Statement statement = connection.createStatement();

            statement.executeUpdate(queryCreateVaccinationTable);
            num++;

            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {

            Statement statement = connection.createStatement();

            statement.executeUpdate(queryCreateAllergicSideEffectTable);
            num++;

            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {

            Statement statement = connection.createStatement();

            statement.executeUpdate(queryCreateSeenTable);
            num++;

            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return num;
    }

    @Override
    public int dropTables() {

        int num = 0;
        String queryDropSeenTable = "drop table if exists Seen";
        String queryDropVaccinationTable = "drop table if exists Vaccination";
        String queryDropUserTable = "drop table if exists User";
        String queryDropVaccineTable = "drop table if exists Vaccine";
        String queryDropAllergicSideEffectTable = "drop table if exists AllergicSideEffect";

        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(queryDropVaccinationTable);
            num++;

            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(queryDropSeenTable);
            num++;

            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(queryDropUserTable);
            num++;

            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(queryDropVaccineTable);
            num++;

            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }


        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(queryDropAllergicSideEffectTable);
            num++;

            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return num;

    }

    @Override
    public int insertUser(User[] users) {
        int num = 0;
        for (int i = 0; i < users.length; i++) {
            String query = "insert into User values (\"" +
                    users[i].getUserID() + "\",\"" +
                    users[i].getUserName() + "\",\"" +
                    users[i].getAge() + "\",\"" +
                    users[i].getAddress() + "\",\"" +
                    users[i].getPassword() + "\",\"" +
                    users[i].getStatus() + "\")";

            try {
                Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                num++;

                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return num;
    }

    @Override
    public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects) {
        int num = 0;
        for (int i = 0; i < sideEffects.length; i++) {
            String query = "insert into AllergicSideEffect values (\"" +
                    sideEffects[i].getEffectCode() + "\",\"" +
                    sideEffects[i].getEffectName() + "\")";
            try {
                Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                num++;

                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return num;
    }

    @Override
    public int insertVaccine(Vaccine[] vaccines) {
        int num = 0;
        for (int i = 0; i < vaccines.length; i++) {
            String query = "insert into Vaccine values (\"" +
                    vaccines[i].getCode() + "\",\"" +
                    vaccines[i].getVaccineName() + "\",\"" +
                    vaccines[i].getType() + "\")";
            try {
                Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                num++;

                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return num;
    }

    @Override
    public int insertVaccination(Vaccination[] vaccinations) {
        int num = 0;
        for (int i = 0; i < vaccinations.length; i++) {
            String query = "insert into Vaccination values (\"" +
                    vaccinations[i].getCode() + "\",\"" +
                    vaccinations[i].getUserID() + "\",\"" +
                    vaccinations[i].getDose() + "\",\"" +
                    vaccinations[i].getVacdate() + "\")";
            try {
                Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                num++;

                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return num;
    }

    @Override
    public int insertSeen(Seen[] seens) {
        int num = 0;
        for (int i = 0; i < seens.length; i++) {
            String query = "insert into Seen values (\"" +
                    seens[i].getEffectcode() + "\",\"" +
                    seens[i].getCode() + "\",\"" +
                    seens[i].getUserID() + "\",\"" +
                    seens[i].getDate() + "\",\"" +
                    seens[i].getDegree() + "\")";

            try {
                Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                num++;

                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return num;
    }

    @Override
    public Vaccine[] getVaccinesNotAppliedAnyUser() {

        ArrayList<Vaccine> tmpList = new ArrayList<Vaccine>();
        Vaccine arr[];
        Vaccine tmp;
        ResultSet rs;
        String query = "select V.code, V.vaccinename, V.type " +
                "from Vaccine V " +
                "where V.code not in (select code " +
                "from Vaccination) " +
                "order by code asc ";
        try {
            Statement statement = connection.createStatement();
            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("code");
                String tmp_vaccinename = rs.getString("vaccinename");
                String tmp_type = rs.getString("type");
                tmp = new Vaccine(tmp_code, tmp_vaccinename, tmp_type);
                tmpList.add(tmp);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        arr = new Vaccine[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }

    //3.4 List users who have been vaccinated for two doses since the given date (5 pts)
    //Input: vacdate
    //Output: userID, userName, address
    //Order the results by userID in ascending order
    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersforTwoDosesByDate(String vacdate) {

        ArrayList<QueryResult.UserIDuserNameAddressResult> tmpList = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult arr[];
        QueryResult.UserIDuserNameAddressResult tmp;
        ResultSet rs;

        String query="select distinct U.userID, U.userName, U.address " +
                "from User U " +
                "where U.userID in (select Vn.userID " +
                "from Vaccination Vn " +
                "where Vn.vacdate>=(select STR_TO_DATE(\"" +
                vacdate+
                "\",\"%Y-%m-%d\")) " +
                "group by Vn.userID " +
                "having count(*)=2) " +
                "order by U.userID asc " ;

        try {
            Statement statement = connection.createStatement();

            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("userID");
                String tmp_vaccinename = rs.getString("userName");
                String tmp_type = rs.getString("address");
                tmp = new QueryResult.UserIDuserNameAddressResult(tmp_code, tmp_vaccinename, tmp_type);
                tmpList.add(tmp);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        arr = new QueryResult.UserIDuserNameAddressResult[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }

    /*
    3.5 List of the two recent vaccines applied to any user that do not contain ‘vac’ in their name (10 pts)
Output: code, vaccinename, type
Order the results by code in ascending order
     */
    @Override
    public Vaccine[] getTwoRecentVaccinesDoNotContainVac() {

        ArrayList<Vaccine> tmpList = new ArrayList<Vaccine>();
        Vaccine arr[];
        Vaccine tmp;
        ResultSet rs;
        String query="select tmp2.code, tmp2.vaccinename, tmp2.type from(select distinct tmp.code, V.vaccinename, V.type "+ //, tmp.vaccinename, tmp.type " +
                "from (select distinct Vn.code,Vn.vacdate " +
                "from Vaccination Vn " +
                "order by Vn.vacdate desc) tmp, Vaccine V " +
                "where tmp.code=V.code and V.vaccinename not like \"%vac%\" " +
                "limit 2) tmp2 " +
                "order by tmp2.code asc ";

        try {
            Statement statement = connection.createStatement();
            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("code");
                String tmp_vaccinename = rs.getString("vaccinename");
                String tmp_type = rs.getString("type");
                tmp = new Vaccine(tmp_code, tmp_vaccinename, tmp_type);
                tmpList.add(tmp);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        arr = new Vaccine[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }

    //3.6 List users who have had at least two doses of vaccine and have experienced at most one side effect (10 pts)
    //Output: userID, userName, address
    //Order the results by userID in ascending order
    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersAtHasLeastTwoDoseAtMostOneSideEffect() {
        ArrayList<QueryResult.UserIDuserNameAddressResult> tmpList = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult arr[];
        QueryResult.UserIDuserNameAddressResult tmp;
        ResultSet rs;

        String query="select U.userID,U.userName,U.address " +
                "from User U " +
                "where U.userID in (select Vn.userID " +
                "from Vaccination Vn " +
                "where Vn.dose>=2) " +
                "and U.userID not in (select S.userID " +
                "from Seen S " +
                "group by S.userID " +
                "having count(*)>1) " +
                "order by U.userID asc";

        try {
            Statement statement = connection.createStatement();

            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("userID");
                String tmp_vaccinename = rs.getString("userName");
                String tmp_type = rs.getString("address");
                tmp = new QueryResult.UserIDuserNameAddressResult(tmp_code, tmp_vaccinename, tmp_type);
                tmpList.add(tmp);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        arr = new QueryResult.UserIDuserNameAddressResult[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }


    //3.7 List users who have been vaccinated with all vaccines that can cause the given side effect (10 pts)
    //Input: effectname
    //Output: userID, userName, address
    //Order the results by userID in ascending order
    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersWithAllVaccinesCanCauseGivenSideEffect(String effectname) {
        ArrayList<QueryResult.UserIDuserNameAddressResult> tmpList = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult arr[];
        QueryResult.UserIDuserNameAddressResult tmp;
        ResultSet rs;

        String query="select U.userID,U.userName,U.address " +
                "from User U " +
                "where not exists (select Vn.userID " +
                "from Vaccination Vn " +
                "where Vn.userID=U.userID and not exists(select S.userID " +
                "from Seen S, AllergicSideEffect A " +
                "where A.effectName=\"" +
                effectname+
                "\" and S.effectcode=A.effectcode and Vn.code=S.code)) " +
                "order by U.userID ";

        try {
            Statement statement = connection.createStatement();

            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("userID");
                String tmp_vaccinename = rs.getString("userName");
                String tmp_type = rs.getString("address");
                tmp = new QueryResult.UserIDuserNameAddressResult(tmp_code, tmp_vaccinename, tmp_type);
                tmpList.add(tmp);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        arr = new QueryResult.UserIDuserNameAddressResult[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersWithAtLeastTwoDifferentVaccineTypeByGivenInterval(String startdate, String enddate) {

        String query= "select U.userID, U.userName, U.address " +
                "from User U, Vaccination V1, Vaccination V2 " +
                "where (V1.vacdate>=(select STR_TO_DATE( \"" +
                startdate+
                "\",\"%Y-%m-%d\")) and V1.vacdate<=(select STR_TO_DATE(\"" +
                enddate+
                "\",\"%Y-%m-%d\"))) and (V2.vacdate>=(select STR_TO_DATE( \""+
                startdate+
                "\",\"%Y-%m-%d\")) and V2.vacdate<=(select STR_TO_DATE(\"" +
                enddate+
                "\",\"%Y-%m-%d\"))) and V1.userID=V2.userID and U.userID=V1.userID and "+
                "V1.code <> V2.code " +
                "order by U.userID asc";

        try {
            Statement statement = connection.createStatement();

            ResultSet rs =  statement.executeQuery(query);

            if (rs.next()) {
                return new QueryResult.UserIDuserNameAddressResult[]{
                        new QueryResult.UserIDuserNameAddressResult(
                                rs.getInt("userID"),
                                rs.getString("userName"),
                                rs.getString("address")
                        )
                };
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    /*
    3.9 List side effects experienced by users who have two doses of vaccine with less than 20 days between doses (10 pts)
     */
    @Override
    public AllergicSideEffect[] getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays() {

        ArrayList<AllergicSideEffect> tmpList = new ArrayList<AllergicSideEffect>();
        AllergicSideEffect arr[];
        AllergicSideEffect tmp;
        ResultSet rs;

        String query="select A.effectcode, A.effectname " +
                "from AllergicSideEffect A, Vaccination V1, Vaccination V2, Seen S " +
                "where A.effectcode=S.effectcode and S.userID=V1.userID and S.userID=V2.userID and V1.dose-V2.dose=1 and " +
                "(select DATEDIFF(V1.vacdate,V2.vacdate))<20 " +
                "order by A.effectcode asc";

        try {
            Statement statement = connection.createStatement();
            rs = statement.executeQuery(query);
            while (rs.next()) {
                int tmp_code = rs.getInt("effectcode");
                String tmp_vaccinename = rs.getString("effectname");
                tmp = new AllergicSideEffect(tmp_code, tmp_vaccinename);
                tmpList.add(tmp);
            }
        }
        catch(SQLException e) {
            e.printStackTrace();
        }
        arr = new AllergicSideEffect[tmpList.size()];
        arr = tmpList.toArray(arr);
        return arr;
    }


    /*
    3.10 Calculate the average number of doses of vaccinated users over the age of 65 (5 pts)
Output: the average result of applied doses for the given condition
     */
    @Override
    public double averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld() {
        double n = 0;
        ResultSet rs;
        String query = "select avg(tmp.total) as Average " +
                "from (select max(Vn.dose) as total " +
                "from User U, Vaccination Vn " +
                "where U.age>65 and U.userID=Vn.userID " +
                "group by U.userID) as tmp";
        try{
            Statement statement=connection.createStatement();
            rs=statement.executeQuery(query);
            rs.next();
            n=rs.getDouble("Average");
        }
        catch (SQLException e) {
            e.printStackTrace();
        }


        return n;
    }

    @Override
    public int updateStatusToEligible(String givendate) {
        int n=0;
        String query="update User U "+
                "set U.status=\"Eligible\" "+
                "where U.status=\"Not_Eligible\" and (select DATEDIFF((select STR_TO_DATE(\""+
                givendate+
                "\",\"%Y-%m-%d\")),(select max(Vn.vacdate) " +
                "from Vaccination Vn " +
                "where Vn.userID=U.userID)))>=120";


        try{
            Statement statement=connection.createStatement();
            n=statement.executeUpdate(query);
        }
        catch (SQLException e) {
            e.printStackTrace();
        }
        return n;
    }

    @Override
    public Vaccine deleteVaccine(String vaccineName) {
        Vaccine tmp=new Vaccine(0,null,null);
        ResultSet rs;
        String query1="select distinct V.vaccinename,V.code,V.type " +
                "from Vaccine V " +
                "where V.vaccinename=\"" +
                vaccineName+
                "\"";
        String query = "delete from Vaccine " +
                "where vaccineName = \"" +
                vaccineName +
                "\";";

        try {
            Statement statement = connection.createStatement();
            rs = statement.executeQuery(query1);
            rs.next();
            int tmp_code = rs.getInt("code");
            String tmp_vaccinename = rs.getString("vaccinename");
            String tmp_type = rs.getString("type");
            tmp = new Vaccine(tmp_code, tmp_vaccinename, tmp_type);
            statement.close();
            Statement statement2 = connection.createStatement();
            statement2.executeUpdate(query);


            statement2.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return tmp;
    }
}
