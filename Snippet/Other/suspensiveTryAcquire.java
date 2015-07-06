public void suspensiveTryAcquire(int necessitaAcquisto) {
	
	if (!sG.tryAcquire(necessitaAcquisto)) { //acquisisco necessitaAcquisto risorse
		
			System.out.println("Grossista: " + Thread.currentThread().getName() + ". Semaforo rosso per me! Qt non disponibiLi in magazzino.");
			sG.acquire(necessitaAcquisto);
			System.out.println("Grossista: " + Thread.currentThread().getName() + ". Semaforo diventato verde per me! Arrivata disponibilità.");
			
	}
	
	return;
}

