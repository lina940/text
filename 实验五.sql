-- =============================================
-- 实验题目：存储过程的创建、修改、使用和删除
-- 数据库：学生管理数据库
-- =============================================

USE 学生管理数据库;
GO

-- =============================================
-- 1. 创建存储过程 pro_a1
-- 查看成绩表，显示学号、姓名、课程号、课程名、成绩
-- =============================================
IF OBJECT_ID('pro_a1', 'P') IS NOT NULL
    DROP PROCEDURE pro_a1;
GO

CREATE PROCEDURE pro_a1
AS
BEGIN
    SELECT x.学号, x.姓名, c.课程号, k.课程名, c.成绩
    FROM cj c
    INNER JOIN xsqk x ON c.学号 = x.学号
    INNER JOIN kc k ON c.课程号 = k.课程号;
END;
GO

-- 执行查看结果
PRINT '===== pro_a1 执行结果 =====';
EXEC pro_a1;
GO


-- =============================================
-- 2. 创建存储过程 pro_a2
-- 查看平均分及格（>=60）的学生的学号、姓名、课程号、课程名、成绩
-- =============================================
IF OBJECT_ID('pro_a2', 'P') IS NOT NULL
    DROP PROCEDURE pro_a2;
GO

CREATE PROCEDURE pro_a2
AS
BEGIN
    SELECT x.学号, x.姓名, c.课程号, k.课程名, c.成绩
    FROM cj c
    INNER JOIN xsqk x ON c.学号 = x.学号
    INNER JOIN kc k ON c.课程号 = k.课程号
    WHERE x.学号 IN (
        SELECT 学号
        FROM cj
        GROUP BY 学号
        HAVING AVG(成绩) >= 60
    );
END;
GO

-- 执行查看结果
PRINT '===== pro_a2 执行结果 =====';
EXEC pro_a2;
GO


-- =============================================
-- 3. 修改存储过程 pro_a2
-- 用于查看平均分及格的学生的个人信息和所有科目成绩
-- =============================================
ALTER PROCEDURE pro_a2
AS
BEGIN
    SELECT x.学号, x.姓名, x.性别, x.出生日期, x.专业,
           c.课程号, k.课程名, c.成绩
    FROM cj c
    INNER JOIN xsqk x ON c.学号 = x.学号
    INNER JOIN kc k ON c.课程号 = k.课程号
    WHERE x.学号 IN (
        SELECT 学号
        FROM cj
        GROUP BY 学号
        HAVING AVG(成绩) >= 60
    )
    ORDER BY x.学号, c.课程号;
END;
GO

-- 执行查看结果
PRINT '===== 修改后 pro_a2 执行结果 =====';
EXEC pro_a2;
GO


-- =============================================
-- 4. 创建存储过程 pro_a3
-- 有输入参数 @xh 学号，查看某学生的学号、姓名、性别、成绩平均分
-- =============================================
IF OBJECT_ID('pro_a3', 'P') IS NOT NULL
    DROP PROCEDURE pro_a3;
GO

CREATE PROCEDURE pro_a3
    @xh VARCHAR(20)
AS
BEGIN
    SELECT x.学号, x.姓名, x.性别, AVG(c.成绩) AS 平均分
    FROM xsqk x
    INNER JOIN cj c ON x.学号 = c.学号
    WHERE x.学号 = @xh
    GROUP BY x.学号, x.姓名, x.性别;
END;
GO

-- 执行查看不同结果
PRINT '===== pro_a3 查询 001 号学生 =====';
EXEC pro_a3 @xh = '001';
GO
PRINT '===== pro_a3 查询 002 号学生 =====';
EXEC pro_a3 @xh = '002';
GO


-- =============================================
-- 5. 创建存储过程 pro_a4
-- 输入参数 @num，如果女生人数 <= @num 就打印"女生少于X人"
-- 否则打印"女生还是很多的"
-- =============================================
IF OBJECT_ID('pro_a4', 'P') IS NOT NULL
    DROP PROCEDURE pro_a4;
GO

CREATE PROCEDURE pro_a4
    @num INT
AS
BEGIN
    DECLARE @female_count INT;

    SELECT @female_count = COUNT(*)
    FROM xsqk
    WHERE 性别 = '女';

    IF @female_count <= @num
        PRINT '女生少于' + CAST(@num AS VARCHAR(10)) + '人';
    ELSE
        PRINT '女生还是很多的';
END;
GO

-- 执行查看不同结果
PRINT '===== pro_a4 @num = 0 =====';
EXEC pro_a4 @num = 0;
GO
PRINT '===== pro_a4 @num = 5 =====';
EXEC pro_a4 @num = 5;
GO


-- =============================================
-- 6. 创建存储过程 pro_a5
-- 输入参数 @a、@b、@c，输出参数 @mix，输出其中最小的数
-- =============================================
IF OBJECT_ID('pro_a5', 'P') IS NOT NULL
    DROP PROCEDURE pro_a5;
GO

CREATE PROCEDURE pro_a5
    @a INT,
    @b INT,
    @c INT,
    @mix INT OUTPUT
AS
BEGIN
    SET @mix = CASE
        WHEN @a <= @b AND @a <= @c THEN @a
        WHEN @b <= @a AND @b <= @c THEN @b
        ELSE @c
    END;
END;
GO

-- 执行查看不同结果
PRINT '===== pro_a5 测试 =====';
DECLARE @result INT;

EXEC pro_a5 @a = 10, @b = 5, @c = 8, @mix = @result OUTPUT;
PRINT '最小数(10,5,8) = ' + CAST(@result AS VARCHAR(10));

EXEC pro_a5 @a = 3, @b = 7, @c = 1, @mix = @result OUTPUT;
PRINT '最小数(3,7,1) = ' + CAST(@result AS VARCHAR(10));

EXEC pro_a5 @a = 6, @b = 6, @c = 9, @mix = @result OUTPUT;
PRINT '最小数(6,6,9) = ' + CAST(@result AS VARCHAR(10));
GO


-- =============================================
-- 7. 创建存储过程 pro_a6
-- 输出参数 @num0，计算女生人数并输出
-- =============================================
IF OBJECT_ID('pro_a6', 'P') IS NOT NULL
    DROP PROCEDURE pro_a6;
GO

CREATE PROCEDURE pro_a6
    @num0 INT OUTPUT
AS
BEGIN
    SELECT @num0 = COUNT(*)
    FROM xsqk
    WHERE 性别 = '女';
END;
GO

-- 执行查看结果
PRINT '===== pro_a6 执行结果 =====';
DECLARE @femaleCount INT;
EXEC pro_a6 @num0 = @femaleCount OUTPUT;
PRINT '女生人数 = ' + CAST(@femaleCount AS VARCHAR(10));
GO


-- =============================================
-- 8. 创建存储过程 pro_a7
-- 输入参数 @xh 学号，输出参数 @avg0 平均分
-- 打印 "@xh xx同学，平均分是@avg0"
-- =============================================
IF OBJECT_ID('pro_a7', 'P') IS NOT NULL
    DROP PROCEDURE pro_a7;
GO

CREATE PROCEDURE pro_a7
    @xh VARCHAR(20),
    @avg0 DECIMAL(5,2) OUTPUT
AS
BEGIN
    DECLARE @name NVARCHAR(20);

    -- 计算平均分
    SELECT @avg0 = AVG(CAST(成绩 AS DECIMAL(5,2)))
    FROM cj
    WHERE 学号 = @xh;

    -- 获取姓名
    SELECT @name = 姓名 FROM xsqk WHERE 学号 = @xh;

    -- 打印结果
    PRINT @xh + ' ' + ISNULL(@name, '未知') + '同学，平均分是' + CAST(@avg0 AS VARCHAR(10));
END;
GO

-- 执行查看不同结果
PRINT '===== pro_a7 查询 001 =====';
DECLARE @avg1 DECIMAL(5,2);
EXEC pro_a7 @xh = '001', @avg0 = @avg1 OUTPUT;
GO
PRINT '===== pro_a7 查询 002 =====';
DECLARE @avg2 DECIMAL(5,2);
EXEC pro_a7 @xh = '002', @avg0 = @avg2 OUTPUT;
GO
PRINT '===== pro_a7 查询 004 =====';
DECLARE @avg3 DECIMAL(5,2);
EXEC pro_a7 @xh = '004', @avg0 = @avg3 OUTPUT;
GO


-- =============================================
-- 9. 创建存储过程 pro_a8
-- 输入参数 @sex: 0=输出女生人数, 1=输出男生人数, 其他=输出-1
-- =============================================
IF OBJECT_ID('pro_a8', 'P') IS NOT NULL
    DROP PROCEDURE pro_a8;
GO

CREATE PROCEDURE pro_a8
    @sex INT,
    @numsex INT OUTPUT
AS
BEGIN
    IF @sex = 0
        SELECT @numsex = COUNT(*) FROM xsqk WHERE 性别 = '女';
    ELSE IF @sex = 1
        SELECT @numsex = COUNT(*) FROM xsqk WHERE 性别 = '男';
    ELSE
        SET @numsex = -1;
END;
GO

-- 执行查看不同结果
PRINT '===== pro_a8 测试(0=女生) =====';
DECLARE @s1 INT;
EXEC pro_a8 @sex = 0, @numsex = @s1 OUTPUT;
PRINT '女生人数 = ' + CAST(@s1 AS VARCHAR(10));

PRINT '===== pro_a8 测试(1=男生) =====';
DECLARE @s2 INT;
EXEC pro_a8 @sex = 1, @numsex = @s2 OUTPUT;
PRINT '男生人数 = ' + CAST(@s2 AS VARCHAR(10));

PRINT '===== pro_a8 测试(其他值=99) =====';
DECLARE @s3 INT;
EXEC pro_a8 @sex = 99, @numsex = @s3 OUTPUT;
PRINT '结果 = ' + CAST(@s3 AS VARCHAR(10));
GO


-- =============================================
-- 10. 删除今天创建的所有存储过程
-- =============================================
PRINT '===== 删除今天创建的所有存储过程 =====';

DECLARE @procName NVARCHAR(255);
DECLARE @sql NVARCHAR(500);

DECLARE proc_cursor CURSOR FOR
    SELECT name
    FROM sys.objects
    WHERE type = 'P'
      AND CONVERT(DATE, create_date) = CONVERT(DATE, GETDATE());

OPEN proc_cursor;

FETCH NEXT FROM proc_cursor INTO @procName;

WHILE @@FETCH_STATUS = 0
BEGIN
    SET @sql = 'DROP PROCEDURE ' + @procName;
    PRINT '删除: ' + @procName;
    EXEC sp_executesql @sql;
    FETCH NEXT FROM proc_cursor INTO @procName;
END

CLOSE proc_cursor;
DEALLOCATE proc_cursor;
GO

-- 验证删除结果
PRINT '===== 剩余存储过程 =====';
SELECT name AS 剩余存储过程, create_date AS 创建日期
FROM sys.objects
WHERE type = 'P'
ORDER BY name;
GO
