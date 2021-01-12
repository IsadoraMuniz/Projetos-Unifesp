import java.util.concurrent.Semaphore;

public class TrafficController {
    static Semaphore sem;

    public TrafficController() {
        TrafficController.sem = new Semaphore(1, true);
    }

    public void enterLeft() throws InterruptedException{
        TrafficController.sem.acquire();
    }

    public void enterRight() throws InterruptedException{
        TrafficController.sem.acquire();
    }

    public void leaveLeft(){
        TrafficController.sem.release();
    }
    
    public void leaveRight(){
        TrafficController.sem.release();
    }

}
