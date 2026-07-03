/**
 * 多线程售票模拟程序
 * 4个销售点同时售卖共享票池
 */
class TicketPool {
    private int tickets;     // 剩余票数

    public TicketPool(int tickets) {
        this.tickets = tickets;
    }

    // 同步方法：购票
    public synchronized boolean buyTicket(String salesPoint) {
        if (tickets <= 0) {
            System.out.println(salesPoint + "：票已售罄！");
            return false;
        }
        tickets--;    // 票数减1
        System.out.println(salesPoint + " 成功购票1张，剩余：" + tickets + "张");
        return true;
    }

    public int getTickets() {
        return tickets;
    }
}

// 销售点线程
class SalesPoint extends Thread {
    private TicketPool pool;       // 共享票池
    private String name;           // 销售点名称
    private int buyCount;          // 该销售点想购买的票数

    public SalesPoint(TicketPool pool, String name, int buyCount) {
        this.pool = pool;
        this.name = name;
        this.buyCount = buyCount;
    }

    @Override
    public void run() {
        for (int i = 0; i < buyCount; i++) {
            // 模拟购票操作耗时
            try {
                Thread.sleep((int)(Math.random() * 500 + 100));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            // 尝试购票
            if (!pool.buyTicket(name)) {
                break;   // 没票了就停止
            }
        }
    }
}

public class TicketSystem {
    public static void main(String[] args) {
        System.out.println("========== 多线程售票系统 ==========");

        // 创建共享票池，共10张票
        TicketPool pool = new TicketPool(10);

        // 创建4个销售点线程，每个都想买5张票
        SalesPoint s1 = new SalesPoint(pool, "销售点-A", 5);
        SalesPoint s2 = new SalesPoint(pool, "销售点-B", 5);
        SalesPoint s3 = new SalesPoint(pool, "销售点-C", 5);
        SalesPoint s4 = new SalesPoint(pool, "销售点-D", 5);

        // 同时启动4个销售点
        s1.start();
        s2.start();
        s3.start();
        s4.start();

        // 等待所有销售点完成
        try {
            s1.join();
            s2.join();
            s3.join();
            s4.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("\n========== 售罄！共售出10张票 ==========");
    }
}
