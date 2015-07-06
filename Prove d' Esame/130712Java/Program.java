import java.util.ArrayList;


public class Program {

	public static void main(String[] args) {
		
		Azienda azienda = new Azienda(100); //Azienda con capacità del cesto pari a 100
		
		ArrayList<Thread> threads = new ArrayList<Thread>();
		threads.add(new Calzolaio(azienda));
		threads.add(new Calzolaio(azienda));
		threads.add(new Calzolaio(azienda));
		threads.add(new Calzolaio(azienda));
		threads.add(new Operaio(azienda));

		for (Thread thread : threads) {
			
			thread.start();
		
		}
		
	}

}
