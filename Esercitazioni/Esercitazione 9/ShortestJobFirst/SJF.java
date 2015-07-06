package shortestJobFirst;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;

public class SJF {

	public static void main(String[] args) {
	
		Random random = new Random(System.currentTimeMillis());
		Gestore gestore = new Gestore(10000);
		
		ArrayList<ThreadP> threadsP = new ArrayList<ThreadP>();
		
		threadsP.add(new ThreadP(gestore, random, 3000));
		threadsP.add(new ThreadP(gestore, random, 3000));
		threadsP.add(new ThreadP(gestore, random, 3000));
		threadsP.add(new ThreadP(gestore, random, 3000));
		threadsP.add(new ThreadP(gestore, random, 3000));

		Collections.shuffle(threadsP);
		
		for (ThreadP threadP : threadsP) {
			threadP.start();
		}
	
	}

}
