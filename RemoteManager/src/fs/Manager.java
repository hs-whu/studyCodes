package fs;

import java.io.File;

public class Manager {

    private File currentDir = new File(System.getProperty("user.home"));

    public File getCurrentDir() {
        return currentDir;
    }

    public void setCurrentDir(File currentDir) {
        this.currentDir = currentDir;
    }
}
