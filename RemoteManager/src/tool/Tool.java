package tool;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tool {

    public static List<String> paraSeparate(String para){
        List<String> list = new ArrayList<>();

        String regex = "(\"[\\S ]*?\")|([\\S]+\\b)";
        Matcher matcher = Pattern.compile(regex).matcher(para);

        int index = 0;
        while (matcher.find(index)){
            index = matcher.end();
            if(para.charAt(matcher.start()) == '\"'){
                list.add(para.substring(matcher.start() + 1, matcher.end() - 1).strip());
            }else {
                list.add(para.substring(matcher.start(), matcher.end()));
            }
        }
        return list;
    }
}
