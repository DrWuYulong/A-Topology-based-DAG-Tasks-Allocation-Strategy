function path = find_path(topologies)
%本函数将DAG任务内拓扑结构分解成路径，
path = {};
%%%%%%初始化2个栈，o表示未搜索节点，p表示已经搜索路径

%根节点初始化，将所有源节点压栈o
stack_o = find(sum(topologies) == 0);
stack_p = [];
index_temp = 1;
while ~isempty(stack_o)%只要未分配的节点栈不为空
    %弹出栈顶节点，压入到路径栈当中
    current_p = stack_o(end);%当前弹出节点       
    stack_o(length(stack_o)) = [];
    stack_p = [stack_p current_p];
    %然后将弹出节点的子节点压栈到未分配的顶点栈中
    stack_o = [stack_o find(topologies(current_p,:) == 1)];

    %%%%判断是否到路径到了结束节点
    if sum(topologies(stack_p(end),:)) == 0%p栈顶元素到了末尾节点
        path = [path stack_p];%%%记录该条路径
        stack_p(end) = [];%路径末尾节点弹出
        
        %只要路径栈不是空的，并且p栈顶元素不是o栈顶元素的父节点就弹出，回溯到新的未搜索到路径的根节点
        while ~isempty(stack_p) && ~isempty(stack_o) && topologies(stack_p(end),stack_o(end)) == 0
            stack_p(end) = [];
        end
    end
end

end