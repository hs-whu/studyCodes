<%--
  Created by IntelliJ IDEA.
  User: hs151
  Date: 2020/5/10
  Time: 23:55
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" %>
<%@ include file="/WEB-INF/jsp/base.jsp" %>

<!DOCTYPE HTML>
<html lang="cn">

<head>
    <title>guide</title>
    <meta content="width=device-width, initial-scale=1" name="viewport"/>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type"/>
    <meta content="" name="keywords"/>

    <link href="${cssRoot}bootstrap.css" media="all" rel="stylesheet" type="text/css"/>
    <link href="${cssRoot}style1.css" media="all" rel="stylesheet" type="text/css"/>

    <style>
        body{
            background-color:#E0FFFF;
        }
        div.header{
            background-color: #7aba7b;
        }
    </style>
</head>

<body>
<div class="header">
    <div class="container">
        <nav class="navbar navbar-default">
            <div class="container-fluid">

                <div class="navbar-brand">
                    <h1><a href="${webRoot}">远程管理系统</a></h1>
                </div>

                <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
                    <ul class="nav navbar-nav">
                        <li class="active"><a href="${webRoot}guide">目录指引<br><i class="sr-osnly">(current)</i></a></li>
                        <li><a href="${webRoot}fs/view">文件管理</a></li>
                    </ul>
                </div>

            </div>
        </nav>
    </div>
</div>

<div class="post-section">
    <br><br><br>
    <div class="container">
        <h3>选择你要进行的操作</h3>
        <div class="post-grids">
            <div class="col-md-4 post-grid">
                <a class="mask" href="${webRoot}fs/view"><img alt="/" class="img-responsive zoom-img" src="${imgRoot}p1.png"></a>
                <a href="${webRoot}fs/view"><h4>文件管理</h4></a>
            </div>
            <div class="clearfix"></div>
        </div>
    </div>
</div>
</body>
</html>
