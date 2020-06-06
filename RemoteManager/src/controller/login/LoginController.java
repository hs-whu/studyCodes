package controller.login;

import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.yaml.snakeyaml.Yaml;
import tool.Login;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

@Controller
public class LoginController {

    @RequestMapping("postLogin")
    public String postLogin(HttpServletRequest request, ModelMap modelMap, String account, String password){
        if(account.equals("") || password.equals("")){
            modelMap.addAttribute("error", "账号或密码为空");
            return "index";
        }

        try{
            Yaml yaml = new Yaml();
            File file = new File(System.getProperty("root"), "config/login.yaml");
            if(file.exists()){
                Login login = yaml.loadAs(new FileInputStream(file), Login.class);
                if(account.equals(login.getAccount()) && password.equals(login.getPassword())){
                    HttpSession session = request.getSession();
                    session.setAttribute("login", "true");
                    return "redirect: /guide";
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            modelMap.addAttribute("error", "系统错误");
            return "index";
        }

        modelMap.addAttribute("error", "账号或密码错误");
        return "index";
    }

    @RequestMapping("guide")
    public String guide(HttpServletRequest request){
        if("true".equals(request.getSession().getAttribute("login"))){
            return "WEB-INF/jsp/guide";
        }

        return "redirect: /";
    }

    @RequestMapping({"*"})
    public String login(){
        return "redirect: /";
    }
}
