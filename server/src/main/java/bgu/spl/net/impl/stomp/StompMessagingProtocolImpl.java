package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.rci.Command;
import bgu.spl.net.srv.Connections;
import java.util.HashMap;
import java.util.Map;


public class StompMessagingProtocolImpl  implements StompMessagingProtocol<String> {
    private  ConnectionsImpl<String> connections;
    private int  connectionId;
    private boolean connectedFrameRecived=false;
    private boolean shouldTerminate=false;
    private boolean disConnect=false;
    public StompMessagingProtocolImpl() {
    }

    @Override
    public void start(int connectionId, Connections<String> connections) {
      this.connections=(ConnectionsImpl<String>)connections;
      this.connectionId=connectionId; 
    }

    @Override
    public void process(String message) { 
        String[] lines = message.split("\n"); 
        String command = lines [0];         
        Map <String,String> headers = new HashMap<>(); 
        int currLine=1;
        while (currLine<lines.length&&!lines[currLine].isEmpty()){ 
            String [] keyValue = lines[currLine].split(":");
            headers.put(keyValue[0],keyValue[1]);
            currLine++;
        }
        currLine++;
        String body="";
        for (;currLine<lines.length;currLine++){
            body=body+"\n" + lines[currLine];
        }
        String ansFrame="";
        switch(command)
    {
        case"CONNECT":
        ansFrame= connect(lines, headers, currLine, message);
        break;
        case"STOMP":
        ansFrame= connect(lines, headers, currLine, message);
        break;
        case"DISCONNECT":
        ansFrame=disConnect(headers,message); 
        break;          
        case"SEND":     
        ansFrame= send(body,headers,message);
        break;
        case"SUBSCRIBE":
        ansFrame= subscribe(body,headers,message);
        break;
        case"UNSUBSCRIBE":
        ansFrame=unsubscribe(headers,message);
        break;
    }
        if (ansFrame==""){
            createFrame createFrame= new createFrame();
            ansFrame= createFrame.error("", "not in frame format", message, this);      
            
        }    connections.send(connectionId, ansFrame);

     if(ansFrame.substring(0,5).equals("ERROR")||disConnect){
        connections.disconnect(connectionId);
        terminate();
     }
    }


    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    public void terminate (){
        shouldTerminate=true;
    }
    private String connect(String[] lines,Map<String,String> headers, int currLine,String msg){
       String answer= "";
                if (!headers.get("accept-version").equals("1.2")){ 
                    answer = answer + "wrong version";
                }
                if(!headers.get("host").equals("stomp.cs.bgu.ac.il")) {
                    answer= answer + ", wrong host"; 
                }
                 String check=connections.nameFitToPasscode(headers.get("login"), headers.get("passcode"));

                if (check=="error")
                    answer=answer + "name doesnt fit the passcode";
                if (check=="new user"){
                    connections.addNewUser(headers.get("login"), headers.get("passcode"));
                }
                String reciptId = headers.get("receipt");
                createFrame createFrame = new createFrame();
            
                if (answer.length()!=0) {
                    return createFrame.error(reciptId,answer,msg,this);
                }
                connections.addAll(headers.get("login"), headers.get("passcode"), connectionId);
                connectedFrameRecived=true;
                return createFrame.connected(); 
            }

     private String disConnect(Map<String,String> headers,String msg){
        createFrame createFrame = new createFrame();
        String reciptId = headers.get("receipt");
        if(!connectedFrameRecived){
         return createFrame.error(reciptId, "no connected frame received",msg ,this);
        }
        return createFrame.receipt(reciptId);

     }

     private String send (String body, Map<String,String> headers, String msg) {
        createFrame createFrame = new createFrame();
        String dest = headers.get("destination");
        String reciptId = headers.get("receipt");
        if(!connectedFrameRecived){
            return createFrame.error(reciptId, "no connected frame received",msg,this );
           }
        if (!connections.channelExist(dest)){
            return createFrame.error(reciptId, "channel doesnt exist",msg ,this);
        }
        if (!connections.channelMember(dest, connectionId )) {
            return createFrame.error(reciptId, "not a member in this topic", msg,this);
        }
        connections.send(dest, body);
        return createFrame.receipt(reciptId);
     }

     private String subscribe (String body, Map<String,String> headers, String msg) {
        createFrame createFrame = new createFrame();
        String dest = headers.get("destination");
        String subId = headers.get("id");
        String reciptId = headers.get("receipt");
        if(!connectedFrameRecived){
            return createFrame.error(reciptId, "no connected frame received",msg,this );
         }
        if (connections.channelMember(dest, connectionId)){
            return createFrame.error(reciptId, "already member of this channel",msg ,this);
        }
        connections.subscription(connectionId, dest, subId);
        return createFrame.receipt(reciptId);
     }
     private String unsubscribe (Map<String,String> headers, String msg) {
        createFrame createFrame = new createFrame();
        String subId = headers.get("id");
        String reciptId = headers.get("receipt");
        if(!connectedFrameRecived){
            return createFrame.error(reciptId, "no connected frame received",msg,this);
         }
         if(connections.unsubscribe(subId, connectionId)){
            return createFrame.receipt(reciptId);
         }
        return createFrame.error(reciptId, "not successful unsubscribtion",msg,this );

     }

   

              
}
