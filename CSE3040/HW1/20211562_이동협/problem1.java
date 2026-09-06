package java__hw1;

public class problem1 {
	public static void main(String[] args) {
		int die1, die2, sum;
		
		do {
			die1 = (int)(6*Math.random()) + 1;
			die2 = (int)(6*Math.random()) + 1;
			sum = die1 + die2;
			System.out.println("(" + die1 + "," + die2 + ")");
		}
		while(sum != 7);
			
	}
}
