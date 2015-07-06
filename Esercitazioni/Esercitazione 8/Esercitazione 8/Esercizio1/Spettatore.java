package Esercizio1;

import java.util.Random;

public class Spettatore extends Thread {

	private String name;
	private String surname;
	private int age;
	private Teatro t;

	public Spettatore(String name, String surname, int age, Teatro t) {
		
		this.name = name;
		this.surname = surname;
		this.age = age;
		this.t = t;
		
	}

	public String getname() {
		return name;
	}

	public String getSurname() {
		return surname;
	}

	public int getAge() {
		return age;
	}

	/**
	 * ANOMALIA: Perchè se viene chiamata sleep(0)
	 * Lo spettatore entra a teatro anche se non ci
	 * sono più posti disponibili?
	 * SOLUZIONE: Imposto 1000 come tempo di sleep minimo
	 */
	@Override
	public void run() {
		
		boolean entrato = t.entra();
		
		if (entrato){
			//dormi per un numero di secondi arbitrario tra 0 e 3 secondi
			try {
				Random r = new Random(System.currentTimeMillis());
				int rand = (1 + r.nextInt(10))*1000;
				System.out.println(rand);
				Thread.sleep(rand);
				
			} catch (InterruptedException e) {
				
				e.printStackTrace();
				
			}
				
			t.esci();
			
			
		} else {
			
			System.out.println("Thread " + this.getName() + 
									". Impossibile per me entrare a Teatro!" + 
											System.getProperty("line.separator"));
			
		}
		
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return "Nome: " + this.getName() + 
				"\tCognome: " + this.getSurname() +
				"\tEtà: " + this.getAge() + "\n\r";
		
	}

}
