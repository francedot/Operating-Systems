package cuochi;

public class AiutoChef extends Thread {

	private Tavolo t;
	private int tipo;
	
	public AiutoChef(Tavolo t, int tipo) {
	
		this.t = t;
		this.tipo = tipo;
		
	}
	
	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				sleep(1000);
				System.out.println("AiutoChef" + tipo +" " + getName() + 
							". Metto l'ingrediente " + tipo + ".");
				if (tipo == 1)
					t.mettiIngr1();
				else if (tipo == 2)
					t.mettiIngr2();
				else
					System.out.println("Errore");
				
				
			}
			
		} catch (InterruptedException e) { }
		
		super.run();
		
		super.run();
	}

}
