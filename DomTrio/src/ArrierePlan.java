import javax.swing.*;

import java.awt.*;

import java.awt.event.*;

public class ArrierePlan extends JFrame {
	 /**
	 * 
	 */
	JButton b1;
	JLabel l1;

	    public ArrierePlan() {

	        setSize(400,400);
	        setVisible(true);

	        setLayout(new BorderLayout());

	        JLabel background=new JLabel(new ImageIcon("board.bmp"));

	        add(background);

	        background.setLayout(new FlowLayout());

	        l1=new JLabel("Here is a button");
	        b1=new JButton("I am a button");

	        background.add(l1);
	        background.add(b1);
	    }

}
