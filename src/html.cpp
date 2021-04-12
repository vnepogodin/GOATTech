#include <vnepogodin/html.hpp>

#include <memory>   // for allocator_traits<>::value_type
#include <sstream>  // for operator<<, basic_ostream, stringstream

constexpr const char* html_str = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
    <title>Add React in One Minute</title>
  </head>
  <body>

    <h2>Add React in One Minute</h2>
    <p>This page demonstrates using React with no build tooling.</p>
    <p>React is loaded as a script tag.</p>

    <!-- We will put our React component inside this div. -->
    <div id="like_button_container"></div>

    <!-- Load React. -->
    <!-- Note: when deploying, replace "development.js" with "production.min.js". -->
    <script src="https://unpkg.com/react@17/umd/react.development.js" crossorigin></script>
    <script src="https://unpkg.com/react-dom@17/umd/react-dom.development.js" crossorigin></script>

    <script src="https://unpkg.com/@babel/standalone/babel.min.js"></script>

    <!-- Load our React component. -->
    <script type="text/babel">
'use strict';

class LikeButton extends React.Component {
  constructor(props) {
    super(props);
    this.state = { liked: false };
  }

  render() {
    if (this.state.liked) {
      return 'You liked this.';
    }

    return (
      <button onClick={() => this.setState({ liked: true }) }>
        Like
      </button>
    );
  }
}

let domContainer = document.querySelector('#like_button_container');
ReactDOM.render(<LikeButton />, domContainer);
</script>
</body>
</html>)";

const char* get_html_str() {
    return html_str;
}

std::string file_contents_to_html(const std::vector<file_info>& files) {
    std::stringstream ss;
    std::string::size_type len = files[0].url.length();

    ss << get_html_str() << '\n';

    return ss.str();
}
