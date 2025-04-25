#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int ft_strlen(char *str) {
	int i = 0;
	while (str[i])
		i++;
	return (i);
}

void ft_putstr_fd2(char *str) {
	write(2, str, ft_strlen(str));
}

int ft_exec(char **av, char **env, int i, int tmp_fd) {
	av[i] = NULL;
	dup2(tmp_fd, 0);
	close(tmp_fd);
	execve(av[0], av, env);
	ft_putstr_fd2("error: cannot execute ");
	ft_putstr_fd2(av[0]);
	ft_putstr_fd2("\n");
	return (1);
}

int main(int ac, char **av, char **env) {
	int i = 0;
	int status = 0;
	int tmp_fd = dup(0);
	int pid;
	int fd[2];
	(void)ac;
	
	while (av[i] && av[i + 1]) {
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (!strcmp(av[0], "cd")) {
			if (i != 2)
				ft_putstr_fd2("error: cd: bad arguments\n");
			else if (chdir(av[1]) == -1) {
				ft_putstr_fd2("error: cd: cannot change directory to ");
				ft_putstr_fd2(av[1]);
				ft_putstr_fd2("\n");			
			}
		}
		else if (i != 0 && (av[i] == NULL || !strcmp(av[i], ";"))) {
			pid = fork();
			if (pid == -1)
				return 1;
			else if (pid == 0) {
				if (ft_exec(av, env, i, tmp_fd))
					return 1;
			}
			else {
				close(tmp_fd);
				waitpid(pid, &status, 0);
				tmp_fd = dup(0);
			}
		}
		else if (i != 0 && !strcmp(av[i], "|")) {
			pipe(fd);
			pid = fork();
			if (pid == 0) {
				dup2(fd[1], 1);
				close(fd[0]);
				if (ft_exec(av, env, i, tmp_fd))
					return 1;
			}
			else {
				close(tmp_fd);
				close(fd[1]);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return WIFEXITED(status) && WEXITSTATUS(status);
}
