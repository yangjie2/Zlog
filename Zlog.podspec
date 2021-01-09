Pod::Spec.new do |s|
    s.name             = 'Zlog'
    s.version          = '1.0.1'
    s.summary          = 'Zlog is a lightweight case logging system based on mobile platform.'

    s.homepage         = 'https://github.com/yangjie2/Zlog'
    s.license          = { :type => 'MIT', :file => 'LICENSE' }
    s.author           = { 'yangjie' => 'yangjie2107@qq.com'}
    s.source           = { :git => 'https://github.com/yangjie2/Zlog.git', :tag => s.version.to_s }

    s.ios.deployment_target = '9.0'
    s.osx.deployment_target = '10.9'

    s.libraries = 'z'

    s.source_files = "apple/*", "src/*.{h,c,cc}","src/micro-ecc-master/*.{h,c,inc}"
    s.public_header_files = "apple/*.h"
end
