import java.util.Random;


public class Pedone extends Thread {

	private Monitor m;
	private Random r;

	public Pedone(Monitor m) {
		
		this.m = m;
		r = new Random();
		
	}

	/**
	 * Vuole accedere all'isola ma deve chiedere il permesso
	 * al monitor. Strutturazione ciclica:
	 * La carrozzina chiede di entrare nella direzione 0 (verso l'isola) (op. entry)
	 * , occupa l'isola per un pò ed infine esce nel verso opposto
	 */
	@Override
	public void run() {
		
		try {
			
			while (true) {
			
				m.entraPontePA();
				
				//tempo di percorrenza
				sleep(1000*r.nextInt(3));
				
				m.esciPontePA();
				//sono dentro il ponte e ci permango per un pò
				
				sleep(1000*r.nextInt(5));
				
				m.entraPontePR();
				
				//tempo di percorrenza
				sleep(1000*r.nextInt(3));
				
				m.esciPontePR();
				//sono fuori dal ponte e dall'isola
				sleep(1000*r.nextInt(7)); //prima della prossima iterazione
			
			}
			
		} catch (InterruptedException e) {

			e.printStackTrace();
		}
		
		super.run();
	}
	
	


}
