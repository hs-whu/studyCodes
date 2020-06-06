<%--
  Created by IntelliJ IDEA.
  User: hs151
  Date: 2020/5/10
  Time: 13:10
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" %>
<%@ include file="/WEB-INF/jsp/base.jsp" %>

<!DOCTYPE HTML>
<html lang="en" class="no-js">
<head>
    <meta charset="utf-8">
    <title>Login</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">
    <meta name="author" content="">

    <link rel="stylesheet" href="${cssRoot}supersized.css">
    <link rel="stylesheet" href="${cssRoot}style.css">
    <script src="${jsRoot}jquery-2.1.4.min.js"></script>
    <script src="${jsRoot}supersized.3.2.7.min.js"></script>
    <script src="${jsRoot}scripts.js"></script>
    <style>
        body{
            background-image: url("${imgRoot}background.jpg");
        }
    </style>
</head>

<body>
<div class="page-container">
    <br><br><br><br><br>
    <h1>远程管理系统登录</h1>
    <form action="${webRoot}postLogin" method="post">
        <label>
            <input type="text" name="account" class="username" placeholder="账号">
            <input type="password" name="password" class="password" placeholder="密码">
            <button type="submit">登录</button>
        </label>
        <div class="error"><span>+</span></div>
        <div>${error}</div>
    </form>
</div>
</body>
</html>


