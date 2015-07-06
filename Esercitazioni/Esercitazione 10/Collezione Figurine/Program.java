import java.util.ArrayList;


public class Program {

	public static void main(String[] args) {
		
		Monitor m = new Monitor(60);
		ArrayList<Thread> threads = new ArrayList<Thread>();
		
		//TEST 1: CASUALE
//		threads.add(new Collezionista(m, 10));
//		threads.add(new Collezionista(m, 10));
//		threads.add(new Collezionista(m, 10));
//		threads.add(new Collezionista(m, 10));
//		threads.add(new Collezionista(m, 10));
//		threads.add(new Collezionista(m, 10));
		
		//TEST 2: 1 COLLEZIONISTA CON TUTTE LE SUE CARTE RARE E UNO CON ANCHE NORMALI
		threads.add(new Collezionista(m, 10));
		threads.add(new Collezionista(m, 10));
		threads.add(new Collezionista(m, 10));
		threads.add(new Collezionista(m, 10));
		threads.add(new Collezionista(m, 60));
		threads.add(new Collezionista(m, 60));
		threads.add(new Collezionista(m, 60));
		threads.add(new Collezionista(m, 60));
		threads.add(new Collezionista(m, 10));
		threads.add(new Collezionista(m, 10));
		
		for (Thread thread : threads) {
			thread.start();
		}
		
		return;
		
	}

}
