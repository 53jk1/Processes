import java.io.IOException;

class Exec {
    public static void main(String[] args) {
        // Defining arguments and starting the process.
        String[] arguments = {
            "C:\\Windows\\System32\\notepad.exe",
            "C:\\Windows\\Temp\\this_file_will_be_there_forever.txt"
        };
        Process p;
        try {
            p = Runtime.getRuntime().exec(arguments);
        } catch (IOException ex) {
            System.err.println("Failed to create new process (not Windows?).");
            return;
        }

        System.out.println("Process started. Waiting for it to exit.");

        // Waiting for the process to end.
        try {
            p.waitFor();
        } catch (InterruptedException ex) {
            // This exception is thrown if another thread aborts this thread.
            // This should never happen because the sample application only has
            // one thread. However, the Java compiler requires this interrupt
            // to be handled.
        }
        System.out.println("The child process has exited.");
    }
}