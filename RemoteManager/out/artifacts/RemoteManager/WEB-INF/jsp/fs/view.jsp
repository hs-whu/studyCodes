<%@ page import="fs.*" %>
<%@ page import="java.io.File" %>
<%@ page import="java.util.ArrayList" %>
<%@ page import="java.util.List" %>
<%--
  Created by IntelliJ IDEA.
  User: hs151
  Date: 2020/5/10
  Time: 22:45
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" %>
<%@ include file="/WEB-INF/jsp/base.jsp" %>

<% Info info = (Info) request.getAttribute("info");
    Os os = info.getOs();
    User user = info.getUser();
    List<Disk> disks = info.getDisks();

    Manager manager = (Manager) request.getAttribute("manager");
    File currentDir = manager.getCurrentDir();
    List<File> dirsList = new ArrayList<>();
    List<File> filesList = new ArrayList<>();
    File[] files = currentDir.listFiles();
    if(files != null){
        for(File file : files){
            if(file.isDirectory())
                dirsList.add(file);
            else
                filesList.add(file);
        }
    }
    dirsList.sort((o1, o2) -> o1.getName().compareToIgnoreCase(o2.getName()));
    filesList.sort((o1, o2) -> o1.getName().compareToIgnoreCase(o2.getName()));
%>

<!DOCTYPE HTML>
<html lang="cn">
<head>
    <title>文件管理</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <meta name="keywords" content=""/>

    <link href="${cssRoot}bootstrap.css" media="all" rel="stylesheet" type="text/css"/>
    <link href="${cssRoot}style1.css" rel="stylesheet" type="text/css" media="all"/>

    <script src="${jsRoot}jquery-2.1.4.min.js"></script>

    <script>
        function changeDir(tag) {
            $.ajax({
                url : "${webRoot}fs/changeDir",
                type : "POST",
                data : {dir: tag.value},
                async: true,
                cache: false,
                contentType: "application/x-www-form-urlencoded",
                processData: true,
                success : function(result) {
                    if(result) {
                        $("body").html(result);
                    }
                },
                error: function (error) {
                    alert("错误码: " + error.status + "\n" + "错误信息: " + error.statusText);
                }
            });
        }

        function openDir(tag) {
            $.ajax({
                url : "${webRoot}fs/openDir",
                type : "POST",
                data : {dirName: tag.innerHTML},
                async: true,
                cache: false,
                contentType: "application/x-www-form-urlencoded",
                processData: true,
                success : function(result) {
                    if(result) {
                        $("body").html(result);
                    }
                },
                error: function (error) {
                    alert("错误码: " + error.status + "\n" + "错误信息: " + error.statusText);
                }
            });
        }
        
        function parentDir() {
            $.ajax({
                url : "${webRoot}fs/parentDir",
                type : "GET",
                data : false,
                async: true,
                cache: false,
                contentType: "application/x-www-form-urlencoded",
                processData: true,
                success : function(result) {
                    if(result) {
                        $("body").html(result);
                    }
                },
                error: function (error) {
                    alert("错误码: " + error.status + "\n" + "错误信息: " + error.statusText);
                }
            });
        }

        function upload(tag) {
            if(tag.value == null || tag.value === ""){
                alert("文件为空");
            }else{
                let formData = new FormData(tag.parentElement);
                $.ajax({
                    url : "${webRoot}fs/upload",
                    type : "POST",
                    data : formData,
                    async: true,
                    cache: false,
                    contentType: false,
                    processData: false,
                    success : function(result) {
                        if(result) {
                            $("body").html(result);
                        }
                    },
                    error: function (error) {
                        alert("错误码: " + error.status + "\n" + "错误信息: " + error.statusText);
                    }
                });
            }
        }

        function downloadFile(tag) {
            let ret = confirm("确认下载: " + tag.innerHTML + "?");
            if(ret === true){
                let form = document.getElementById("downloadFileForm");
                form.getElementsByTagName("input")[0].value = tag.innerHTML;
                form.submit();
            }
        }

        function execute(tag) {
            let form = tag.parentElement;
            let exeName = form.getElementsByTagName("input")[0].value;
            let exePara = form.getElementsByTagName("input")[1].value;
            let ret = confirm("确认执行: " + exeName + "?");
            if(ret === true){
                $.ajax({
                    url : "${webRoot}fs/execute",
                    type : "POST",
                    data : {exeName: exeName, exePara: exePara},
                    async: true,
                    cache: false,
                    contentType: "application/x-www-form-urlencoded",
                    processData: true,
                    success : function(result) {
                        if(result) {
                            alert(result);
                        }
                    },
                    error: function (error) {
                        alert("错误码: " + error.status + "\n" + "错误信息: " + error.statusText);
                    }
                });
            }
        }
        
        function remindDir(event) {
            let div = document.getElementById("hidden");
            div.style.left = (event.clientX + 10) + "px";
            div.style.top = (event.clientY) + "px";
            div.innerHTML = "点击打开目录";
            div.style.display = "block";
        }

        function remindFile(event) {
            let div = document.getElementById("hidden");
            div.style.left = (event.clientX + 10) + "px";
            div.style.top = (event.clientY) + "px";
            div.innerHTML = "点击下载文件";
            div.style.display = "block";
        }

        function remindLeave() {
            let div = document.getElementById("hidden");
            div.style.display = "none";
        }
    </script>

    <style type="text/css" media="all">
        body {
            background-color: #E0FFFF;
        }

        form{
            margin: 0;
            display: inline;
        }

        div.header {
            background-color: #7aba7b;
        }

        div.info {
            width: 20%;
            float: left;
        }

        div.content > div.container {
            width: 80%;
            float: right;
        }

        table {
            width: 100%;
            height: 100%;
            background-color: white;
            border: 1px solid #000000;
        }

        tr {
            border: 1px solid #000000;
        }

        th, td {
            border: 1px solid #000000;
            text-align: left;
            padding: 5px 6px;
            width: 20%;
        }

        tr.title{
            background-color: #e5e5e5;
        }

        tr.dir:hover{
            background-color: #FAE6A3;
        }

        tr.file:hover{
            background-color: #d4d2d5;
        }

        td.dir:hover{
            cursor: pointer;
            text-decoration: underline;
            color: blue;
        }

        td.file:hover{
            cursor: pointer;
            text-decoration: underline;
            color: blue;
        }

        h4 {
            margin-bottom: 5px;
        }

        span.error{
            color: red;
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
                        <li><a href="${webRoot}guide">目录指引</a></li>
                        <li class="active"><a href="${webRoot}fs/view">文件管理<br><i class="sr-osnly">(current)</i></a>
                        </li>
                    </ul>
                </div>

            </div>
        </nav>
    </div>
</div>

<div class="content" style="background-color: #3071a9">
    <div class="info">
        <br>
        <h4>操作系统信息:</h4>
        <ul>
            <li>操作系统名：<%= os.getName() %></>
            <li>操心系统架构：<%= os.getArch()%></li>
            <li>操心系统版本：<%= os.getVersion()%></li>
        </ul>

        <br>
        <h4>用户信息：</h4>
        <ul>
            <li>用户名：<%= user.getName()%></li>
            <li>用户主目录：<%= user.getHome()%></li>
            <li>当前工作目录：<%= user.getDir()%></li>
        </ul>

        <br>
        <h4>硬盘分区信息</h4>
        <% for (Disk disk : disks) { %>
        <ul>
            <li>路径：<%= disk.getPath()%></li>
            <li>空间大小：<%= disk.getTotalSpace()%></li>
            <li>已用空间：<%= disk.getUsedSpace()%></li>
            <li>可用空间：<%= disk.getFreeSpace()%></li>
        </ul>
        <br>
        <% } %>
    </div>

    <div class="container">
        <br>
        当前目录：<%= currentDir.getAbsolutePath()%>&emsp;&emsp;控制台信息: <span class="error">${error}</span>
        <br>
        <% for (Disk disk : disks){%>
        <button onclick="changeDir(this)" value="<%=disk.getPath()%>">切换<%=disk.getPath().substring(0,1)%>盘</button>
        <%}%>
        <button onclick="parentDir()">返回上一级</button>
        <form id="downloadFileForm" method="get" action="${webRoot}fs/downloadFile" style="display: none">
            <label>
                <input id="fileName" name="fileName" type="text">
            </label>
        </form>
        &emsp;&emsp;
        <form>
            <label>执行程序:</label>
            <label for="processName">程序:</label>
            <input type="text" name="processName" id="processName">
            <label for="para">参数:</label>
            <input type="text" name="para" id="para">
            <button onclick="execute(this)">提交</button>
        </form>
        &emsp;&emsp;
        <form>
            <label>上传文件:</label>
            <input type="file" id="file" name="file" onchange="upload(this)" placeholder="上传文件">
        </form>

        <br>
        <div>
            <table>
                <tr class="title">
                    <th>文件名</th>
                    <th>类型</th>
                    <th>大小</th>
                </tr>
                <% for(File file : dirsList) { %>
                <tr class="dir">
                    <td class="dir" onmouseover="remindDir(event || window.event)" onmouseleave="remindLeave()"
                        onmousemove="remindDir(event || window.event)" onclick="openDir(this)"><%= file.getName()%></td>
                    <td>目录</td>
                    <td></td>
                </tr>
                <% }%>
                <% for(File file : filesList) { %>
                <tr class="file">
                    <td  class="file" onmouseover="remindFile(event || window.event)" onmouseleave="remindLeave()"
                         onmousemove="remindFile(event || window.event)" onclick="downloadFile(this)"><%= file.getName()%></td>
                    <td>文件</td>
                    <td><%= file.length() > 1048576 ? file.length() / 1048576 + "M" : (file.length() > 1024 ?
                            file.length() / 1024 + "K" : file.length() + "B")%></td>
                </tr>
                <% }%>
            </table>
        </div>
    </div>
</div>

<div id="hidden" style="display: none; position: fixed; background-color: #89fffc"></div>

</body>
</html>
