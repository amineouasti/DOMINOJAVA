package domino;

import java.sql.Connection;
import java.sql.*;

public class Main {

	public static void main(String[] args) {
		Identification identifiant=new Identification();
		System.out.println(identifiant.get_pseudo());
		System.out.println(identifiant.get_mot_de_passe());
		
		try {

		      Class.forName("org.oracle.Driver");
		      String url = "jdbc:oracle://localhost:1521/essai";
		      String user = "postgres";
		      String passwd = "";
		       
		      Connection conn = DriverManager.getConnection(url, user, passwd);
		      Statement state = conn.createStatement();
		      ResultSet result = state.executeQuery("SELECT * FROM identifiant");
		      ResultSetMetaData resultMeta = result.getMetaData();		         
		      System.out.println("\n**********************************");
		      

		      for(int i = 1; i <= resultMeta.getColumnCount(); i++)
		        System.out.print("\t" + resultMeta.getColumnName(i).toUpperCase() + "\t *");
		      System.out.println("\n**********************************");
		      while(result.next()){         
		        for(int i = 1; i <= resultMeta.getColumnCount(); i++)
		          System.out.print("\t" + result.getObject(i).toString() + "\t |");		            
		        System.out.println("\n---------------------------------");
		      }
		      result.close();
		      state.close();
		    } catch (Exception e) {
		      e.printStackTrace();
		    }	     
		}	  
}

