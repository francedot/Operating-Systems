import java.util.Random;


public class Calzolaio extends Thread {
	
	private int tipo;
	
	private Azienda azienda;
	private Random r;
	
	public Calzolaio(Azienda azienda) {
		
		this.azienda = azienda;
		r = new Random();
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				//calzolaio decide che tipo di scarpa fare
				tipo = r.nextInt(5);
				
				//effettua operazione di deposito della scarpa
				azienda.deposito(tipo);
				sleep(r.nextInt(3)*1000);
				
			}
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
		
		}
		
		super.run();
	}
	
}
