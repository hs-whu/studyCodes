package fs;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class Info {

    private Os os;

    private User user;

    private List<Disk> disks;

    void init(){
        this.os = new Os();
        os.setName(System.getProperty("os.name"));
        os.setArch(System.getProperty("os.arch"));
        os.setVersion(System.getProperty("os.version"));

        this.user = new User();
        user.setName(System.getProperty("user.name"));
        user.setHome(System.getProperty("user.home"));
        user.setDir(System.getProperty("user.dir"));

        disks = new ArrayList<>();
        File[] roots = File.listRoots();
        for(File rootFile : roots){
            Disk disk = new Disk();
            disk.setPath(rootFile.getPath());
            disk.setTotalSpace(rootFile.getTotalSpace() / 1024 / 1024 / 1024 + "G");
            disk.setFreeSpace(rootFile.getFreeSpace() / 1024 / 1024 / 1024 + "G");
            disk.setUsedSpace((rootFile.getTotalSpace() - rootFile.getFreeSpace()) / 1024 / 1024 / 1024 + "G");
            disks.add(disk);
        }
    }


    public Os getOs() {
        return os;
    }

    public void setOs(Os os) {
        this.os = os;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public List<Disk> getDisks() {
        return disks;
    }

    public void setDisks(List<Disk> disks) {
        this.disks = disks;
    }
}

