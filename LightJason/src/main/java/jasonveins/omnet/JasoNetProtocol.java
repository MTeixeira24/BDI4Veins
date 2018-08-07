package jasonveins.omnet;

public class JasoNetProtocol {
    public enum State{
        REGISTER
    }

    State state;

    public JasoNetProtocol(){
        state = State.REGISTER;
    }

    public String processInput( String input ){
         return "OK";
    }


}
