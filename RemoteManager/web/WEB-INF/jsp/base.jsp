<%--
  Created by IntelliJ IDEA.
  User: hs151
  Date: 2020/5/11
  Time: 11:29
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ taglib uri="http://www.springframework.org/tags" prefix="table" %>
<%
    String path = request.getContextPath();
    String basePath = request.getScheme() + "://" + request.getServerName() + ":" + request.getServerPort() + path + "/";
    String cssPath = basePath + "static/css/";
    String jsPath = basePath + "static/js/";
    String imgPath = basePath + "static/img/";
%>
<c:set var="webRoot" value="<%=basePath%>" />
<c:set var="cssRoot" value="<%=cssPath%>" />
<c:set var="jsRoot" value="<%=jsPath%>" />
<c:set var="imgRoot" value="<%=imgPath%>" />
