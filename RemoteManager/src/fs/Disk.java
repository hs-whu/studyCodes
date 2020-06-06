package fs;

public class Disk {

    private String path;

    private String totalSpace;

    private String freeSpace;

    private String usedSpace;

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public String getTotalSpace() {
        return totalSpace;
    }

    public void setTotalSpace(String totalSpace) {
        this.totalSpace = totalSpace;
    }

    public String getFreeSpace() {
        return freeSpace;
    }

    public void setFreeSpace(String freeSpace) {
        this.freeSpace = freeSpace;
    }

    public String getUsedSpace() {
        return usedSpace;
    }

    public void setUsedSpace(String usedSpace) {
        this.usedSpace = usedSpace;
    }
}
