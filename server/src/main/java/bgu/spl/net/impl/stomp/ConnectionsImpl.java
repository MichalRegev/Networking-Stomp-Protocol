package bgu.spl.net.impl.stomp;
//package javafx.util;

import java.io.IOException;
import java.sql.Connection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;



public class ConnectionsImpl<T> implements bgu.spl.net.srv.Connections<T>{
    private AtomicInteger cId=new AtomicInteger(0);
    private Map <Integer,String> id_nameMap= new ConcurrentHashMap<>(); 
    private Map <Integer,String> id_passcodeMap= new ConcurrentHashMap<>(); 
    private Map <Integer,ConnectionHandler<T>> id_connectionHandlerMap= new ConcurrentHashMap<>(); 
    private Map <String,LinkedList <Integer>> channel_chMap= new ConcurrentHashMap<>(); 
    private Map <String,LinkedList <int[]>> channelSubscriptionId= new ConcurrentHashMap<String,LinkedList<int[]>>(); 
    private Map <String,String> name_passcode= new ConcurrentHashMap<>(); 
    AtomicInteger messageId=new AtomicInteger(0);
    public ConnectionsImpl (){      
    }

    @Override
    public boolean send(int connectionId, Object msg) {
        if(id_connectionHandlerMap.get(connectionId)!=null)
        id_connectionHandlerMap.get(connectionId).send((T)msg);
        return false; 
    }

    @Override
    public void send(String channel, Object msg) {
        LinkedList <int[]> channelToSend = channelSubscriptionId.get(channel);
        for (int i=0;i<channelToSend.size();i++){
            int [] currArr=channelToSend.get(i);
            int connectionID= currArr[0];
            int subId = currArr[1];
            createFrame createFrame =new createFrame();
            msg= createFrame.message(subId, messageId.incrementAndGet(), channel, (String)msg);
            id_connectionHandlerMap.get(connectionID).send((T)msg);
        }


    }

    @Override
    public void disconnect(int connectionId) {
        id_connectionHandlerMap.remove(connectionId); 
        id_nameMap.remove(connectionId);
        id_passcodeMap.remove(connectionId);
        Iterator <Map.Entry<String,LinkedList<Integer>>>iterator= channel_chMap.entrySet().iterator();
        while (iterator.hasNext()){
            String key= iterator.next().getKey();
            Integer connectionIdInteger= new Integer(connectionId);
            channel_chMap.get(key).remove(connectionIdInteger);
        }
        Iterator <Map.Entry<String,LinkedList<int[]>>> iter= channelSubscriptionId.entrySet().iterator();
        while (iter.hasNext()){
            String key= iter.next().getKey();
            LinkedList <int[]> currList= channelSubscriptionId.get(key);
            for (int arr=0; arr<currList.size();arr++) {
                if (currList.get(arr)[0]==connectionId){
                    currList.remove(arr);
                }
            }
            Integer connectionIdInteger= new Integer(connectionId);
            channel_chMap.get(key).remove(connectionIdInteger);
        }
        
        }
    
    @Override
    public boolean containName(String name) {
        Iterator <Map.Entry<Integer,String>>iterator= id_nameMap.entrySet().iterator();
        while (iterator.hasNext()){
            Integer key= iterator.next().getKey();
            if (id_nameMap.get(key).equals(name)){
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean containPasscode(String passcode) {
        Iterator <Map.Entry<Integer,String>>iterator= id_passcodeMap.entrySet().iterator();
        while (iterator.hasNext()){
            Integer key= iterator.next().getKey();
            if (id_passcodeMap.get(key).equals(passcode)){
                return true;

            }
        }
        return false;
    }

    public void addConection (ConnectionHandler<T> blockingConnectionHandler,int id) {
        id_connectionHandlerMap.put(id,blockingConnectionHandler);
    }

    public String nameFitToPasscode (String name, String passcode) {
               if (name_passcode.get(name)==null) {
            return "new user";
        }
        if ( name_passcode.get(name).equals(passcode)){
            return "fit";
        }
        return "error";        
    }

    public void addNewUser(String name, String passcode){
        name_passcode.put(name,passcode);
    }
     
    public void addAll(String name ,String passcode, int Id){
     id_nameMap.put(Id,name);
     id_passcodeMap.put(Id, passcode);
    }
 
    public int getId(){
        return cId.incrementAndGet();
    }

    public boolean channelExist (String channel) {
        if(channel_chMap.get(channel)==null){
            return false;
        }
        return true;
    }

    public boolean channelMember (String channel, int id) {
        if(channel_chMap.get(channel)!=null){
        LinkedList<Integer> channelMembers= channel_chMap.get(channel);
        if(channelMembers.contains(id)){
            return true;
        }
        }
        return false;
    }

    public void subscription (int id, String channel, String subId) {
        int subIdI= Integer.parseInt(subId);
        int [] arr = {id,subIdI};
        if (!channelSubscriptionId.containsKey(channel)){
            LinkedList <int[]> newList1 = new LinkedList<>();
            LinkedList <Integer> newList2 = new LinkedList<>();
            newList1.addFirst(arr);
            newList2.addFirst(id);
            channelSubscriptionId.put(channel,newList1);
            channel_chMap.put(channel,newList2);

        }
        else {
            channelSubscriptionId.get(channel).addFirst(arr);
            channel_chMap.get(channel).addFirst(id);

        }
    }
    public boolean unsubscribe(String subId,int connectionID){
        boolean successfulUnsubscribing=false;
        int subIdI= Integer.parseInt(subId);
        Iterator <Map.Entry<String,LinkedList<int[]>>>iterator= channelSubscriptionId.entrySet().iterator();
        while (iterator.hasNext()){
            String key= iterator.next().getKey();
            LinkedList <int[]> currList= channelSubscriptionId.get(key);
            for (int arr=0; arr<currList.size();arr++) {
                if (currList.get(arr)[1]==subIdI&&currList.get(arr)[0]==connectionID){
                    successfulUnsubscribing=true;
                    currList.remove(arr);
                    LinkedList <Integer> List1= channel_chMap.get(key);
                    Integer idI = new Integer(connectionID);
                    List1.remove(idI);
                }
            }        
    }
    return successfulUnsubscribing;
}

    
}
