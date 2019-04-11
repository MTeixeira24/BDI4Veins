package jasonveins.omnet.agent.voting;

import junit.framework.TestCase;

public class CSatisfactionCollectorTest extends TestCase {

    public void testExportCsv() {
        CSatisfactionCollector sc = new CSatisfactionCollector("test.csv",
                "PlatoonSize", "SingleRule", "CommRule", "Density", "Run",
                "SpeedUtility", "RouteUtility");

        sc.addRow(1);
        sc.addRow(2);
        sc.addRow(3);

        sc.setValue(1, "PlatoonSize", String.valueOf(10));
        sc.setValue(1, "SingleRule", "Approval");
        sc.setValue(1, "CommRule", "Minimax");
        sc.setValue(1, "Density", String.valueOf(50));
        sc.setValue(1, "Run", String.valueOf(24));
        sc.setValue(1, "SpeedUtility", String.valueOf(0.812368));
        sc.setValue(1, "RouteUtility", String.valueOf(0.7));



        sc.setValue(2, "PlatoonSize", String.valueOf(10));
        sc.setValue(2, "SingleRule", "Approval");
        sc.setValue(2, "CommRule", "Minimax");
        sc.setValue(2, "Density", String.valueOf(50));
        sc.setValue(2, "Run", String.valueOf(24));
        sc.setValue(2, "SpeedUtility", String.valueOf(0.919287));
        sc.setValue(2, "RouteUtility", String.valueOf(0.2));



        sc.setValue(3, "PlatoonSize", String.valueOf(10));
        sc.setValue(3, "SingleRule", "Approval");
        sc.setValue(3, "CommRule", "Minimax");
        sc.setValue(3, "Density", String.valueOf(50));
        sc.setValue(3, "Run", String.valueOf(24));
        sc.setValue(3, "SpeedUtility", String.valueOf(0.239120));
        sc.setValue(3, "RouteUtility", String.valueOf(0.4));

        sc.exportCsv();

    }
}