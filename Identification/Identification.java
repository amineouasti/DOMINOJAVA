package domino;

import java.awt.BorderLayout;

import java.awt.Color;

import java.awt.Dimension;

import java.awt.event.ActionEvent;

import java.awt.event.ActionListener;

import javax.swing.JOptionPane;



public class Identification {
	JOptionPane jop = new JOptionPane();
    private String pseudo = jop.showInputDialog(null, "Pseudo", "Identification", JOptionPane.QUESTION_MESSAGE);
    private String motDePasse = jop.showInputDialog(null, "Mot de passe", "Identification", JOptionPane.QUESTION_MESSAGE);
    
    public String get_pseudo(){
    	return(pseudo);
    }
    
    public String get_mot_de_passe() {
    	return(motDePasse);
    }
}
