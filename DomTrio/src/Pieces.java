import java.awt.image.BufferedImage;

public class Pieces {
	private int val1;
	private int[][] initialiser;
	//private int val2;
	//private int val3;
	private BufferedImage pieceImage;
	
	/**
	 * DOMINO
	 * @param val1     La valeur contenue dans la partie haute de l'image de la piece
	 * @param val2     La valeur contenue dans la partie bas de l'image de la piece
	 * TRIOMINO
	 * @param val1     La valeur contenue dans la partie haute de l'image de la piece
	 * @param val2     La valeur contenue dans la partie bas-gauche de l'image de la piece
	 * @param val3     La valeur contenue dans la partie bas-droit de l'image de la piece
	 * 
	 * @param Image    L'image associée à chaque piece  
	 */
	public int[][] gettab() {
		return this.initialiser;
	}
	public Pieces(int val1) {
		int a = 0;
		int b = 0;
		int i, j;
		if (val1 == 2) {
			initialiser=new int[28][2];
			for(i=0;i<28;i++)
			{
				for(j=0;j<2;j++)
				{
					if(j==0)
					{
						this.initialiser[i][j] = a;
					}
					else
					{
						this.initialiser[i][j] = b;
						b++;
					}
					if(b==7)
					{
						a++;
						b=a;
					}
				}	
			}
		}
		
	}
		//this.val1 = val1;
		//this.val2 = val2;
		//this.pieceImage = Image;
	/*public int getVal2() {
		return this.val2;
	}
	public int getVal3() {
		return this.val3;
	}
	public BufferedImage affiche() {
		return pieceImage;
	}
	*/
	/*public Pieces(int val1, int val2, BufferedImage Image) {
		this.val1 = val1;
		this.val2 = val2;
		this.pieceImage = Image;
	}
	public Pieces(int val1, int val2, int val3, BufferedImage Image) {
		this.val1 = val1;
		this.val2 = val2;
		this.val3 = val3;
		this.pieceImage = Image;
	}
	*/
}
