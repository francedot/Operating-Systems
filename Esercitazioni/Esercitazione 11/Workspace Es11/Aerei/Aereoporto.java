import java.util.ArrayList;
import java.util.Collections;


public class Aereoporto {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

		Pista p = new Pista();
		ArrayList<Thread> threads = new ArrayList<Thread>();
		
		//TEST 1: 1 AEREO E 4 MEZZIS
		
//		threads.add(new Aereo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
		
		//TEST 2: 1 AEREO E > 10 MEZZI
		
//		threads.add(new Aereo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
		
		//TEST 3: 4 AEREI E 4 MEZZI
		
//		threads.add(new Aereo(p));
//		threads.add(new Aereo(p));
//		threads.add(new Aereo(p));
//		threads.add(new Aereo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
//		threads.add(new Mezzo(p));
		
		//TEST 4: N AEREI E N MEZZI
	
		threads.add(new Aereo(p, "AZ125"));
		threads.add(new Aereo(p, "AZ126"));
		threads.add(new Aereo(p, "AZ127"));
		threads.add(new Aereo(p, "AZ128"));
		threads.add(new Aereo(p, "AZ129"));
		threads.add(new Aereo(p, "AZ130"));
		threads.add(new Aereo(p, "AZ131"));
		threads.add(new Aereo(p, "AZ132"));
		threads.add(new Aereo(p, "AZ133"));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		threads.add(new Mezzo(p));
		
		Collections.shuffle(threads);
		
		for (Thread thread : threads) {
			
			thread.start();
			
		}
		
	}

}
