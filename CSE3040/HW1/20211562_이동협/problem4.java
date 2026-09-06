package java__hw1;

import java.util.Scanner;

public class problem4 {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		String str = sc.nextLine();
		
		int len = str.length();
		
		for(int i = 0 ; i < len; i++) {
			System.out.print(str.charAt(len - i - 1));
		}
	}
}
