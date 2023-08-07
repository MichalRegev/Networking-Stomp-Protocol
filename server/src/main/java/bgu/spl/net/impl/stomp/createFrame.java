package bgu.spl.net.impl.stomp;

public class createFrame {
    
    public createFrame () {

    }
    public String error (String reciptId, String details, String frameCauseError, StompMessagingProtocolImpl p) {
        String errorFrame= "ERROR\n" + "recipt-id: message -" + reciptId+ "\nmessage: " + details + "\n\nThe message:\n-----\n" + frameCauseError; 
        return errorFrame; //messafe id or recipt
    } 

    public String connected () {
        String connectedFrame = "CONNECTED\nversion:1.2\n\n";
        return connectedFrame;
    }

    public String receipt (String reciptId) {
        String receiptFrame = "RECEIPT\n" + "receipt-id:" + reciptId+"\n\n";
        return receiptFrame;
    }

    public String message(int subId, int msgId, String dest, String body) {
        String message= "MESSAGE\n"+ "subscription:" +subId +"\nmessage-id:" +msgId+ "\ndestination:" +dest+"\n" +body +"\n";
        return message;
    }
}
