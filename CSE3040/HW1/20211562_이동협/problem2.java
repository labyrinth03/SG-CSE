package java__hw1;

public class problem2 {
	public static void main(String[] args) {
		
		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < i + 1; j++) {
				System.out.print(" ");
			}
			System.out.print("*");
			for(int j = 0; j < 2*(7-i) + 1; j++) {
				System.out.print(" ");
			}	
			System.out.print("*");
			for(int j = 0; j < i + 1; j++) {
				System.out.print(" ");
			}
			System.out.println();
		}
		
		System.out.println("         *         ");
		
		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8-(i); j++) {
				System.out.print(" ");
			}
			System.out.print("*");
			for(int j = 0; j < 2*i + 1; j++) {
				System.out.print(" ");
			}	
			System.out.print("*");
			for(int j = 0; j < 8-(i + 1); j++) {
				System.out.print(" ");
			}
			System.out.println();
		}		
	}
}
