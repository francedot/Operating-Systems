
public class Piscina {

	public static void main(String[] args) {
		
		Spogliatoio p = new Spogliatoio(25, 15);
		
		//Singolo: può essere uomo o donna
		Singolo s1 = new Singolo(0, p);
		Singolo s2 = new Singolo(1, p);
		Singolo s3 = new Singolo(1, p);
		Singolo s4 = new Singolo(0, p);
		Singolo s5 = new Singolo(0, p);
		Singolo s6 = new Singolo(1, p);
		Singolo s7 = new Singolo(0, p);
		
		Gruppo g1 = new Gruppo(s1, 2, p);

		System.out.println("heila!");
		
	}

}
