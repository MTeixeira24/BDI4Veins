package jasonveins.omnet.voting;

import java.util.Comparator;
import java.util.function.Function;
import java.util.function.ToDoubleFunction;
import java.util.function.ToIntFunction;
import java.util.function.ToLongFunction;

/**
 * Holds a pair utility-index. Used created sorted arrays of utilities
 */
public class CUtilityPair implements Comparator<CUtilityPair>, Comparable<CUtilityPair> {
    private double utility;
    private int index;

    public CUtilityPair(int p_index, double p_utility){
        setIndex(p_index);
        setUtility(p_utility);
    }

    public double getUtility() {
        return utility;
    }

    public void setUtility(double utility) {
        this.utility = utility;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    @Override
    public int compareTo(CUtilityPair o) {
        double diff = this.utility - o.getUtility();
        if(diff > 0) return 1;
        if(diff < 0) return -1;
        return 0;
    }

    @Override
    public int compare(CUtilityPair o1, CUtilityPair o2) {
        double diff = o1.getUtility() - o2.getUtility();
        if(diff > 0) return 1;
        if(diff < 0) return -1;
        return 0;
    }
}
