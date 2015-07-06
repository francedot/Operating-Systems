import java.util.ArrayList;


public class Program {

	public static void main(String[] args) {
		
		//Una lavanderia che contiene L lavatrici e A asciugatrici
		Lavanderia lavanderia = new Lavanderia(40, 40);
		
		ArrayList<Cliente> clienti = new ArrayList<Cliente>();
		clienti.add(new Cliente(lavanderia)); //classe mi setta casualmente il tipo di clientela
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));
		clienti.add(new Cliente(lavanderia));

		for (Cliente cliente : clienti) {
			
			cliente.start();
			
		}
		
	}

}
