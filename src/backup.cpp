#include "task.h"
#include "argparser.h"

int main(int argc, char **argv)
{
    // 检查参数数量
    if (argc < 2)
    {
        std::cout << "Usage: backup [OPTION...]" << std::endl;
        std::cout << "Try 'backup --help' for more information." << std::endl;
        return -1;
    }

    // 解析参数
    ArgParser parser;
    if (!parser.Parse(argc, argv))
    {
        parser.PrintErrorInfo();
        return -1;
    }

    // 打印帮助文档
    if (parser.flag_help)
        return 0;

    // 备份
    if (parser.flag_backup)
    {
        Task task(parser.str_input, parser.str_output); // 创建备份任务

        // 判断备份模式
        unsigned char mod = 0;
        if (parser.flag_compress)
            mod |= BACKUP_MOD_COMPRESS; // 压缩
        if (parser.flag_encrypt)
        {
            mod |= BACKUP_MOD_ENCRYPT; // 加密
            if (!parser.flag_password) // 未输入密码
            {
                std::string pwd1, pwd2;
                std::cout << "Input password: ";
                std::cin >> pwd1;
                std::cout << "Input password again: ";
                std::cin >> pwd2;
                if (pwd1 != pwd2)
                {
                    std::cout << "error: passwords do not match" << std::endl; // 两次输入密码不匹配
                    return -1;
                }

                parser.flag_password = true;
                parser.str_password = pwd2;

                if (!parser.CheckPassword()) // 检验密码格式
                    return -1;
            }
        }

        task.SetMod(mod); // 设置备份模式
        task.SetVerbose(parser.flag_verbose); // 设置是否输出执行过程信息
        task.SetComment(parser.str_message); // 设置备份的备注信息

        // 设置过滤器
        Filter filter;
        if (parser.flag_path)
            filter.SetPathFilter(parser.str_path);
        if (parser.flag_name)
            filter.SetNameFilter(parser.str_name);
        if (parser.flag_type)
            filter.SetFileType(parser.file_type);
        if (parser.flag_atime)
            filter.SetAccessTime(parser.atime_start, parser.atime_end);
        if (parser.flag_mtime)
            filter.SetModifyTime(parser.mtime_start, parser.mtime_end);
        if (parser.flag_ctime)
            filter.SetChangeTime(parser.ctime_start, parser.ctime_end);
        task.SetFilter(filter);

        if (!task.Backup(parser.str_password))
            return -1;
        else
            std::cout<<"Done"<<std::endl;
    }

    // 恢复
    if (parser.flag_restore)
    {
        Task task(parser.str_output, parser.str_input); // 创建恢复任务
        if (!task.GetBackupInfo()) // 获取备份信息
            return -1;
        if ((task.GetBackupMode() & BACKUP_MOD_ENCRYPT) && !parser.flag_password) // 备份文件加密且未输入密码
        {
            std::cout << "Input password: ";
            std::cin >> parser.str_password;
        }
        task.RestoreMetadata(parser.flag_metadata); // 设置是否恢复文件元信息
        task.SetVerbose(parser.flag_verbose); // 设置是否输出执行过程信息
        if (!task.Restore(parser.str_password))
            return -1;
        else
            std::cout<<"Done"<<std::endl;
    }

    // 显示备份文件信息
    if (parser.flag_list)
    {
        BackupInfo info;
        if (Task::GetBackupInfo(parser.str_list, info))
        {
            std::string backup_mode = "pack ";
            if (info.mod & BACKUP_MOD_COMPRESS)
                backup_mode += "compress ";
            if (info.mod & BACKUP_MOD_ENCRYPT)
                backup_mode += "encrypt";
            char backup_time[100];
            strftime(backup_time, sizeof(backup_time), "%Y-%m-%d %H:%M:%S", localtime(&info.timestamp));
            std::cout << "Backup path: " << info.backup_path << std::endl;
            std::cout << "Backup Time: " << backup_time << std::endl;
            std::cout << "Backup Mode: " << backup_mode << std::endl;
            std::cout << "Comment: " << info.comment << std::endl;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}