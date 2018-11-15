package jasonveins.omnet.managers;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

/**
 * Helper class to handle byte operations
 */
public class CByteUtils {

    public static String extractString(ByteBuffer buffer){
        int strlen = buffer.getInt();
        byte[] utf8bytes = new byte[strlen];
        buffer.get(utf8bytes, 0, strlen);
        return new String(utf8bytes, StandardCharsets.UTF_8);
    }
}
