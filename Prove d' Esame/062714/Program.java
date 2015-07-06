import java.util.ArrayList;
import java.util.Collections;

/**
	Sulla spiaggia di una località turistica marittima è disponibile un servizio di noleggio di pattìni a
	remi (detti anche “mosconi”).
	Il servizio è gestito da un addetto e dispone di N pattìni, disponibili per il noleggio ai turisti.
	Ogni pattìno può ospitare al massimo 4 persone e viene noleggiato sempre e soltanto per un’ora.
	
	I turisti si recano a gruppi dall’addetto, che, a seconda del numero di persone che compone il
	gruppo, assegna loro il numero di pattìni corrispondente (se disponibile) altrimenti mette il gruppo
	in attesa che tutti i pattìni richiesti siano disponibili.
	A questo proposito, si assuma la seguente ipotesi semplificativa: un gruppo non può essere
	composto da più di 8 persone.
	
	Al momento del ritiro dei pattìni, ogni gruppo paga all’addetto 4€ per ogni pattìno ottenuto e una
	cauzione di 3€ per ogni pattìno (pertanto, per ogni pattino noleggiato, il gruppo versa 4+3=7€).
	Dopo un’ora di utilizzo, il gruppo ritorna a riva e restituisce i pattìni, ottenendo dall’addetto la
	restituzione della cauzione (3€/pattìno).
	Ogni tanto l’addetto si assenta e chiude il noleggio. Durante l’assenza, se nella cassa vi sono più di
	3 €, si reca in banca per versare una parte dei contanti accumulati fino a quel momento nella cassa;
	a questo proposito si assuma che:
	- l’addetto decida casualmente quando chiudere il noleggio e quando successivamente
	riaprirlo;
	- se l’addetto è assente, il noleggio sia chiuso e pertanto i pattìni non possano essere né
	prelevati, né restituiti;
	- l’addetto versi in banca i contanti accumulati nella cassa meno la quota di una cauzione; ciò
	significa che, in seguito al versamento in banca, la cassa conterrà 3€.
	- i soldi versati in banca non possano essere utilizzati per la restituzione della cauzione ai
	clienti.
	Si realizzi un programma Java nel quale gruppi e addetto siano rappresentati da thread
	concorrenti e che, utilizzando il concetto di monitor e le variabili condizione, realizzi il servizio
	di noleggio tenendo conto dei vincoli dati e, inoltre, del seguente vincolo di priorità:
	•
	•
	nel prelievo dei pattìni sia data la priorità ai gruppi meno numerosi;
	nella restituzione dei pattìni sia data la priorità ai gruppi più numerosi.
**/
public class Program {

	public static void main(String[] args) {
		
		Noleggio noleggio = new Noleggio(10);
		Addetto addetto = new Addetto(noleggio);
		Gruppo g1 = new Gruppo(4, noleggio);
		Gruppo g2 = new Gruppo(5, noleggio);
		Gruppo g3 = new Gruppo(2, noleggio);
		Gruppo g4 = new Gruppo(3, noleggio);
		
		ArrayList<Thread> threads = new ArrayList<Thread>();
		threads.add(addetto);
		
//		threads.add(g1);
//		threads.add(g2);
//		threads.add(g3);
//		threads.add(g4);
		
		//TEST 1: Avvio solo l'addetto
		//addetto.start();
		
		//TEST 1: 1 ADDETTO E 2 GRUPPI
		threads.add(g1);
		
		Collections.shuffle(threads);
		
		for (Thread thread : threads) {
			
			thread.start();
			
		}
		
	}

}
