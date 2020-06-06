package controller.fs;

import fs.Info;
import fs.Manager;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;
import org.springframework.web.multipart.MultipartFile;
import tool.Tool;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.*;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Objects;

@Controller
public class FsController {

    @RequestMapping("view")
    public String view(HttpServletRequest request, ModelMap modelMap){
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Info info = (Info) webApplicationContext.getBean("info");
            modelMap.addAttribute("info", info);

            Manager manager = (Manager) webApplicationContext.getBean("manager");
            modelMap.addAttribute("manager", manager);
            return "view";
        }

        modelMap.addAttribute("error", "系统错误");
        return "view";
    }

    @RequestMapping("changeDir")
    public String changeDir(HttpServletRequest request, ModelMap modelMap, String dir){
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Info info = (Info) webApplicationContext.getBean("info");
            modelMap.addAttribute("info", info);

            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File newDir = new File(dir);
            if(newDir.exists() && newDir.isDirectory()){
                manager.setCurrentDir(newDir);
                modelMap.addAttribute("manager", manager);
                return "view";
            }

            modelMap.addAttribute("manager", manager);
            modelMap.addAttribute("error", "切换目录失败");
            return "view";
        }

        modelMap.addAttribute("error", "系统错误");
        return "view";
    }

    @RequestMapping("openDir")
    public String openDir(HttpServletRequest request, ModelMap modelMap, String dirName) {
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Info info = (Info) webApplicationContext.getBean("info");
            modelMap.addAttribute("info", info);

            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File newDir = new File(manager.getCurrentDir(), dirName);
            if(newDir.exists() && newDir.isDirectory()){
                manager.setCurrentDir(newDir);
                modelMap.addAttribute("manager", manager);
                return "view";
            }

            modelMap.addAttribute("manager", manager);
            modelMap.addAttribute("error", "打开目录失败");
            return "view";
        }

        modelMap.addAttribute("error", "系统错误");
        return "view";
    }

    @RequestMapping("parentDir")
    public String parentDir(HttpServletRequest request, ModelMap modelMap){
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Info info = (Info) webApplicationContext.getBean("info");
            modelMap.addAttribute("info", info);

            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File parentDir = manager.getCurrentDir().getParentFile();
            if(parentDir != null && parentDir.exists()){
                manager.setCurrentDir(parentDir);
                modelMap.addAttribute("manager", manager);
                return "view";
            }

            modelMap.addAttribute("error", "已经是根目录");
            modelMap.addAttribute("manager", manager);
            return "view";
        }

        modelMap.addAttribute("error", "系统错误");
        return "view";
    }

    @RequestMapping("upload")
    public String upload(HttpServletRequest request, ModelMap modelMap, MultipartFile file){
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Info info = (Info) webApplicationContext.getBean("info");
            modelMap.addAttribute("info", info);

            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File newFile = new File(manager.getCurrentDir(), Objects.requireNonNull(file.getOriginalFilename()));
            try(FileOutputStream o = new FileOutputStream(newFile);InputStream i = file.getInputStream()){
                o.write(i.readAllBytes());
            } catch (IOException e) {
                modelMap.addAttribute("error", "写文件失败");
            }

            modelMap.addAttribute("manager", manager);
            return "view";
        }

        modelMap.addAttribute("error", "系统错误");
        return "view";
    }

    @RequestMapping("downloadFile")
    @ResponseBody
    public byte[] downloadFile(HttpServletRequest request, HttpServletResponse response, String fileName){
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File file = new File(manager.getCurrentDir(), fileName);
            if(file.exists()){
                try(FileInputStream fileInputStream = new FileInputStream(file)) {
                    byte[] data = fileInputStream.readAllBytes();
                    fileInputStream.close();
                    response.setHeader("Content-Type", "application/octet-stream");
                    response.setHeader("Content-Disposition", "attachment; filename=" + URLEncoder.encode(fileName, StandardCharsets.UTF_8));
                    return data;
                } catch (IOException ignored) {
                }
            }
        }

        return new byte[0];
    }

    @RequestMapping("execute")
    @ResponseBody
    public byte[] execute(HttpServletRequest request, String exeName, String exePara){

        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getWebApplicationContext(request.getServletContext());
        if(webApplicationContext != null){
            Manager manager = (Manager) webApplicationContext.getBean("manager");
            File file = new File(manager.getCurrentDir(), exeName);
            List<String> list = Tool.paraSeparate(exePara);
            if(file.exists()){
                list.add(0, file.getPath());
            }else {
                list.add(0, exeName);
            }

            try {
                Runtime.getRuntime().exec(list.toArray(new String[0]));
                return ("execute success: " + exeName).getBytes();
            } catch (IOException e) {
                return ("execute fail: " + exeName).getBytes();
            }
        }

        return "system error".getBytes();
    }
}