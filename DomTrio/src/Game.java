import java.awt.BorderLayout;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class Game {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		Pieces piece1 = new Pieces(2);
		int[][] dominos=piece1.gettab();
		
		/*
		//System.out.println(piece1.affiche());
		System.out.println("Valeur UP:" + piece1.getValUp());
		System.out.println("Valeur DOWN:" + piece1.getValDown());
		//new ArrierePlan();*/
		JFrame window = new JFrame("Jeu Domino & Triomino");
		window.setSize(700, 540);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//window.setContentPane(new JLabel(new ImageIcon(ImageIO.read(new File("board.bmp")))));
		//window.setContentPane(new JLabel(new ImageIcon(piece1.affiche())));
		JPanel contentPane = new JPanel(new BorderLayout()); 
		int x=0,y=0;
		for (int i=0 ;i<28;i++) {
			String image="";
			for(int j=0;j<2;j++) {
	
				image+=dominos[i][j];
			}
			image+=".bmp";
			System.out.println(image);
			JLabel pieceLabel = new JLabel(new ImageIcon((ImageIO.read(new File(image)).getScaledInstance(50, 70,100))));
			//pieceLabel.setSize(50,50);
			
			x+=100;
			y+=50;
			pieceLabel.setLocation(x, y);
			contentPane.add(pieceLabel);
			
		}
		window.add(contentPane);
		window.setVisible(true);
		
		
	}

}
