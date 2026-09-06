package hw3;

import java.awt.*;
import java.util.Random;
import java.util.*;
import java.awt.event.*;
import java.lang.Math;
import javax.swing.JOptionPane;
public class assign3_20211562 extends Frame implements ActionListener
{
	
	private Canvas canvas ;
	public assign3_20211562()
	{
		canvas = new Canvas();
		add("Center", canvas);
		Panel p = new Panel();
		Button s = new Button("Run/Pause");
		Button c = new Button("Close");
		p.add(s);p.add(c);
		s.addActionListener(this);
		c.addActionListener(this);
		add("South", p);
	}
	public void actionPerformed(ActionEvent evt)
	{
		if ( evt.getActionCommand () == "Run/Pause")
		{
			Ball b = new Ball(canvas);
			b.start();
		}
		else if (evt.getActionCommand () == "Close") {
			System.exit(0);
		}
	}
	public static void main(String[] args)
	{
		Frame f = new assign3_20211562();
		f.setSize(400,300);
		WindowDestroyer listener = new WindowDestroyer();
		f.addWindowListener(listener);
		f.setVisible(true);
	}


	private Canvas box;
	private static Vector<Integer> XSIZE = new Vector<>(Arrays.asList(16, 16, 16, 16, 16));
	private static Vector<Integer> YSIZE = new Vector<>(Arrays.asList(16, 16, 16, 16, 16));
	private static Vector<Integer> x = new Vector<>(Arrays.asList(150, 160, 180, 200, 210));
	private static Vector<Integer> y = new Vector<>(Arrays.asList(105, 137, 80, 137, 105));
	private static Vector<Integer> dx = new Vector<>(Arrays.asList(-1, -2, 1, 1, 2));
	private static Vector<Integer> dy = new Vector<>(Arrays.asList(-1, 2, -2, 2, -1));
	private static Vector<Integer> dxdy = new Vector<>(Arrays.asList(-4, -3, -2, -1, 1, 2, 3, 4));
	private static int BALLS = 5;
	private static int ChangedBALLS = 5;
	private static int skip = -1;
	private static int stop = 1;
	class Ball extends Thread
	{
		public Ball(Canvas c) { box = c; }
		public void draw()
		{
			Graphics[] g = new Graphics[5000];
			
			for(int i = 0; i < 5; i++) {
				g[i] = box.getGraphics();
				g[i].fillOval(x.get(i), y.get(i), XSIZE.get(i), YSIZE.get(i));
				g[i].dispose(); 				
			}
		}
		public void move()
		{
			Graphics[] g = new Graphics[5000];
			Dimension d = box.getSize();
			
			for(int i = 0; i < BALLS; i++) {
				g[i] = box.getGraphics();
				g[i].setXORMode(box.getBackground());				
				g[i].fillOval(x.get(i), y.get(i), XSIZE.get(i), YSIZE.get(i));
			}
			for(int i = 0; i < BALLS; i++) {
				g[i].clearRect(0, 0, getWidth(), getHeight());
			}			
			
			for(int i = 0; i < BALLS; i++) {
				x.set(i, x.get(i)+dx.get(i));
				y.set(i, y.get(i)+dy.get(i));
				if (x.get(i) < 0) { 
					x.set(i, 0); 
					dx.set(i, -dx.get(i)); 
				}
				if (x.get(i) + XSIZE.get(i) >= d.width) {
					x.set(i, d.width - XSIZE.get(i));
					dx.set(i, -dx.get(i));
				}
				if (y.get(i) < 0) {
					y.set(i, 0); 
					dy.set(i, -dy.get(i)); 
				}
				if (y.get(i) + YSIZE.get(i) >= d.height) {
					y.set(i,d.height - YSIZE.get(i)); 
					dy.set(i, -dy.get(i)); 
				}
				for(int j = 0; j < BALLS; j++) {
					skip = -1;
					int xc2 = x.get(j); int xc1 = x.get(i);
					int yc2 = y.get(j); int yc1 = y.get(i);
					int r2 = XSIZE.get(j)/2; int r1 = XSIZE.get(i)/2; 
					if(i!=j && (Math.pow(xc2 - xc1, 2) + Math.pow(yc2 - yc1, 2) <= Math.pow(r1 + r2, 2))) {	//서로 다른 공이 충돌하면
						//System.out.println("Collison! "+XSIZE.get(i)+"with"+XSIZE.get(j));
						Random rand = new Random();
						///////////////////////////////
						if(XSIZE.get(i)/2 == 0) {			//부딪힌 공i가 0이 돼서 삭제되는 경우 : 아무것도 안 함

						}
						else {								//공i가 쪼개지는 경우 : 공i에서 쪼개진 공을 생성
							x.add(x.get(i) + 4*XSIZE.get(i));
							y.add(y.get(i) + 4*YSIZE.get(i));
							dx.add(dxdy.get(rand.nextInt(dxdy.size())));
							dy.add(dxdy.get(rand.nextInt(dxdy.size())));		
							XSIZE.add(XSIZE.get(i)/2);
							YSIZE.add(YSIZE.get(i)/2);
							ChangedBALLS++;										
						}
						//////////////////////////////
						if(XSIZE.get(j)/2 == 0) {			//부딪힌 공j가 0이 돼서 삭제되는 경우 : 아무것도 안 함
							
						}
						else {								//공j가 쪼개지는 경우 : 공i에서 쪼개진 공을 생성
							x.add(x.get(j) + 4*XSIZE.get(j));
							y.add(y.get(j) + 4*YSIZE.get(j));						
							dx.add(dxdy.get(rand.nextInt(dxdy.size())));
							dy.add(dxdy.get(rand.nextInt(dxdy.size())));													
							XSIZE.add(XSIZE.get(j)/2);
							YSIZE.add(YSIZE.get(j)/2);
							ChangedBALLS++;									
						}
						///////////////////////////////
						if(XSIZE.get(i)/2 == 0) {			//부딪힌 공i의 크기가 0이 돼서 삭제되는 경우
							if(XSIZE.get(j)/2 == 0) {		//i와 j가 둘 다 크기가 1이라 부딪혓을 떄 둘다 없어지는 경우 
								if(i < j) {					//j가 i보다 컸다면 j값이 바뀌기 때문에
									j--;					//j값 정정
								}
								skip = i;
							}
							x.remove(i);					//기존 i공 삭제
							y.remove(i);
							dx.remove(i);
							dy.remove(i);
							XSIZE.remove(i);
							YSIZE.remove(i);
							ChangedBALLS--;		
							
						}
						else {

							dx.set(i, dxdy.get(rand.nextInt(dxdy.size())));
							dy.set(i, dxdy.get(rand.nextInt(dxdy.size())));
							
							XSIZE.set(i, XSIZE.get(i)/2);
							YSIZE.set(i, YSIZE.get(i)/2);							
						}
						//////////////////////////////
						if(XSIZE.get(j)/2 == 0) {			//부딪힌 공j의 크기가 0이 돼서 삭제되는 경우
							x.remove(j);					//기존 j공 삭제
							y.remove(j);
							dx.remove(j);
							dy.remove(j);
							XSIZE.remove(j);
							YSIZE.remove(j);
							ChangedBALLS--;	
							skip = j;
						}
						else {
							dx.set(j, dxdy.get(rand.nextInt(dxdy.size())));
							dy.set(j, dxdy.get(rand.nextInt(dxdy.size())));
							XSIZE.set(j, XSIZE.get(j)/2);
							YSIZE.set(j, YSIZE.get(j)/2);						
						}
						/////////////////////////////	
						break;								//쪼개지면 break;
					}
				}
				if(skip != -1) {							//사라진 공이 있다면 인덱스 초과로 못그릴 수도 있으니 break
					break;
				}
				g[i].fillOval(x.get(i), y.get(i), XSIZE.get(i), YSIZE.get(i));
				g[i].dispose();					
			}
			BALLS = ChangedBALLS;							//공 개수 변동 적용
			//System.out.println(BALLS);
		}
		public void run()
		{
			if(stop == 0) {
				stop = 1;
			}
			else {
				stop = 0;
			}
			if(BALLS > 0) {
				draw();
				while(BALLS >= 1 && stop == 0) 
				{
					move();
					try { Thread.sleep(5); } catch(InterruptedException e) {}

				}
				if(BALLS < 1) {
					System.out.println("Done!");
					JOptionPane.showMessageDialog(null, "Program done with " + BALLS + " Balls", "Alert", JOptionPane.INFORMATION_MESSAGE);					
				}

			}
			else {
				System.out.println("More running is meaningless.");				
			}

		}
		
	}
}
class WindowDestroyer extends WindowAdapter
{
    public void windowClosing(WindowEvent e) 
    {
        System.exit(0);
    }
}