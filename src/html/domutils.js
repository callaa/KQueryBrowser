function domElement(name, ...children) {

    console.log("domElement", name, children);
    const element = document.createElement(name);

    if(children.length > 0 && typeof children[0] !== 'string' && !(children[0] instanceof Node)) {
        const attrs = children.shift();
        for(const [key, value] of Object.entries(attrs)) {
            if(key === 'onclick')
                element.onclick = value;
            else
                element.setAttribute(key, value);
        }
    }

    for(let c of children) {
        if(c == null) {
            continue;
        } else  if(typeof(c) === 'string') {
            c = document.createTextNode(c);
        }
        element.appendChild(c);
    }

    return element;
}
