package cuochi;

public class Chef extends Thread {

	private Tavolo t;
	
	public Chef(Tavolo t) {
	
		this.t = t;
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				sleep(10000);
				System.out.println("Chef " + getName() + ". Voglio preparare il piatto.");
				t.prepara();
				
				
			}
			
		} catch (InterruptedException e) { }
		
		super.run();
		
	}

}
