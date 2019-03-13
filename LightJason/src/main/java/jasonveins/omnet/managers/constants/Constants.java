package jasonveins.omnet.managers.constants;


/**
 * Two-byte length constants that must be agreed upon between the c++ and java side to enable communication
 */
public class Constants {
    public static final short CONNECTION_ACK = 1;
    public static final short AGENT_ADD = 2;
    public static final short AGENT_REMOVE = 3;
    public static final short SET_BELIEF = 4;
    public static final short REQUEST_DECISIONS = 5;
    public static final short SET_SIM_PARAMS = 6;
    public static final short SEND_JOIN_REQUEST = 7;
    public static final short BULK_AGENT_ADD = 8;
    public static final short CONNECTION_END = 126;

    public static final short VALUE_BOOL = 1;

    public static final short VALUE_CHAR = 2;

    public static final short VALUE_SHORT = 3;

    public static final short VALUE_INT = 4;

    public static final short VALUE_LONG = 5;

    public static final short VALUE_FLOAT = 6;

    public static final short VALUE_DOUBLE = 7;

    public static final short VALUE_ARRAY = 8;

    public static final short VALUE_NULL = 9;

    
}
