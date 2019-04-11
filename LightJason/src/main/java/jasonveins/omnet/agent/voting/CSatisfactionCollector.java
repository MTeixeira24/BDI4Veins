package jasonveins.omnet.agent.voting;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class CSatisfactionCollector {

    private HashMap<String, Integer> columnMap;
    private LinkedList<String> columnNames;
    private LinkedList<Integer> columnIds;
    private ConcurrentHashMap<Integer, RowObj> rows;
    private String filename;

    public CSatisfactionCollector(String p_filename, String... p_columns){
        columnMap  = new HashMap<>();
        columnIds = new LinkedList<>();
        columnNames = new LinkedList<>();
        rows = new ConcurrentHashMap<>(40);
        filename = p_filename;
        for(int i = 0; i < p_columns.length; i++){
            columnNames.add(p_columns[i]);
            columnIds.add(i);
            columnMap.put(p_columns[i], i);
        }
    }

    public void addRow(int rowId){
        rows.put(rowId, new RowObj(columnIds));
    }

    public void setValue(int rowId, String columnName, String value){
        while(rows.get(rowId) == null)
            addRow(rowId);
        try{

            rows
                    .get(rowId)
                    .putEntry(
                            columnMap
                                    .get(columnName),
                            value);
        }catch (NullPointerException e){
            e.printStackTrace();
            System.out.println("rowId: " + rowId);
            System.out.println("columnName: " + columnName);
            System.out.println("value: " + value);

            System.out.println("columnMap"+columnMap);
            System.out.println("rows.get(rowId)"+rows.get(rowId));
            System.out.println("rows"+rows);
            System.exit(2);
        }

    }

    private String csvHeader(){
        StringBuilder sb = new StringBuilder();
        for(String col : columnNames){
            sb.append(col);
            sb.append(" ");
        }
        return sb.toString().trim().replace(" ", ",");
    }

    public void exportCsv(){
        try {
            FileWriter out;
            File f = new File("testResults/"+filename);
            if(!f.exists() || f.isDirectory()) {
                out = new FileWriter(f);
                out.write(csvHeader()+"\n");
            }else{
                out = new FileWriter(f, true);
            }
            for(Integer rowId : rows.keySet()){
                String toWrite = rows.get(rowId).toCsv(columnIds)+"\n";
                out.write(toWrite);
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    class RowObj{
        HashMap<Integer, String> rowValues;

        RowObj(List<Integer> p_columns){
            rowValues = new HashMap<>(40);
            if(p_columns != null){
                for(Integer colId : p_columns){
                    putEntry(colId, "0");
                }
            }
        }

        void putEntry(int p_columnId, String p_entryValue){
            rowValues.put(p_columnId, p_entryValue);
        }

        String toCsv(LinkedList<Integer> p_collumnIds){
            StringBuilder sb = new StringBuilder();
            for(Integer id : p_collumnIds){
                sb.append(rowValues.get(id));
                sb.append(" ");
            }
            String t1 = sb.toString();
            t1 = t1.trim();
            t1 = t1.replace(" ", ",");
            return t1;
        }
    }
}
