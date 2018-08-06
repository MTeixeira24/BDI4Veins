package trafficproject.environment;

import javax.annotation.Nonnull;
import java.util.Vector;

public class Node {
    private final String nodeName;
    private final Vector<Link> links = new Vector<>();

    public Node(@Nonnull final String m_nodeName){
        nodeName = m_nodeName;
    }

    public final Vector<Link> links(){
        return  links;
    }

    public final void insertLink(@Nonnull final Link m_link){
        links.add(m_link);
    }
}
