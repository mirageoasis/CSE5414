package kafka;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.ExecutionException;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.PartitionInfo;
import org.apache.kafka.clients.admin.Admin;
import org.apache.kafka.clients.admin.AdminClient;
import org.apache.kafka.clients.admin.CreateTopicsResult;
import org.apache.kafka.clients.admin.KafkaAdminClient;
import org.apache.kafka.clients.admin.NewTopic;
import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;

public class HelloWorld {

    static String SERVER_LOC = "localhost:9092";
    static String topicString = "";
    static String ID_STRING = "";
    static int ID_number = 0;
    static boolean reset = false;

    public static void loginPrompt() {
        System.out.println("Welcome to SogangTalk");
        System.out.println("1. Login");
        System.out.println("2. Exit");
        System.out.println("");

    }

    public static String loginInput() {
        Scanner senario = new Scanner(System.in);
        Scanner name = new Scanner(System.in);

        System.out.print("SogangTalk> ");
        int senario_number = senario.nextInt();

        while (senario_number > 2) {
            System.out.print("SogangTalk> ");
            senario_number = senario.nextInt();
        }

        if (senario_number == 2) {
            System.out.println("Bye!");
            return "";
        }

        System.out.print("SogangTalk> ID: ");
        String ID_RET = name.nextLine();
        System.out.println("");

        return ID_RET;
    }

    public static Set<String> kafkaTopicList() throws InterruptedException, ExecutionException {
        Properties properties = new Properties();
        properties.put("bootstrap.servers", SERVER_LOC);
        Admin admin = Admin.create(properties);
        Set<String> topicNames = admin.listTopics().names().get();
        admin.close();

        return topicNames;
    }

    public static void kafkaTopicCreate(String name) throws FileNotFoundException, IOException {
        Properties config = new Properties();
        config.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, SERVER_LOC);
        config.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, 
                "org.apache.kafka.common.serialization.StringSerializer");
        config.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, 
                "org.apache.kafka.common.serialization.StringSerializer");
        config.put(ProducerConfig.CLIENT_ID_CONFIG, ID_STRING);
        config.put(ProducerConfig.LINGER_MS_CONFIG, 1);
        
        AdminClient adminClient = AdminClient.create(config);
        NewTopic newTopic = new NewTopic(name, 1, (short) 1); // new NewTopic(topicName, numPartitions,
                                                              // replicationFactor)

        List<NewTopic> newTopics = new ArrayList<NewTopic>();
        newTopics.add(newTopic);
        adminClient.createTopics(newTopics);

        adminClient.close();
    }

    public static void chattingRoomPrompt(String name) {
        System.out.println("");
        System.out.println(name);
        System.out.println("1. Read");
        System.out.println("2. Write");
        System.out.println("3. Reset");
        System.out.println("4. Exit");
        System.out.println("");
    }

    public static int chattingRoomInput(String topic_name) {
        Scanner senario = new Scanner(System.in);
        System.out.print("SogangTalk> ");
        int senario_number = senario.nextInt();

        if (senario_number == 1) {
            Properties config = new Properties();
            config.put(ConsumerConfig.GROUP_ID_CONFIG, ID_STRING);
            config.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, SERVER_LOC);
            config.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringDeserializer");
            config.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringDeserializer");
            config.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "earliest");
            config.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "true");

            KafkaConsumer<String, String> consumer = new KafkaConsumer(config);
            consumer.subscribe(Collections.singletonList(topic_name));
            
            try {
                if (reset == true) {
                    consumer.poll(0);
                    consumer.seekToBeginning(consumer.assignment());
                }
                ConsumerRecords<String, String> records = consumer.poll(10000);
                String s;
                for (ConsumerRecord<String, String> record : records) {
                    s = record.value();
                    System.out.println(s);
                }
                reset = false;
            } catch (Exception e) {
                // exception
            } finally {
                consumer.close();
            }

            return senario_number;
        } else if (senario_number == 2) {
            // topic ??????
            System.out.print("SogangTalk> Text: ");
            Scanner chat = new Scanner(System.in);
            String chatString = chat.nextLine();
            chatString = ID_STRING + ": " +chatString;
            Properties config = new Properties();
            config.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, SERVER_LOC);
            config.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringSerializer");
            config.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringSerializer");

            config.put(ProducerConfig.CLIENT_ID_CONFIG, ID_STRING);
            config.put(ProducerConfig.LINGER_MS_CONFIG, 1);
            KafkaProducer<String, String> producer = new KafkaProducer(config);
            ProducerRecord<String, String> record = new ProducerRecord(topic_name, chatString);

            producer.send(record);
            producer.close();
            return senario_number;
        } else if (senario_number == 3) {
            // topic ???????????? ????????? break
            Properties config = new Properties();
            config.put(ConsumerConfig.GROUP_ID_CONFIG, ID_STRING);
            config.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, SERVER_LOC);
            config.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringDeserializer");
            config.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG,
                    "org.apache.kafka.common.serialization.StringDeserializer");
            config.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "earliest");
            config.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "true");

            KafkaConsumer<String, String> consumer = new KafkaConsumer(config);
            consumer.subscribe(Collections.singletonList(topic_name));
            
            consumer.poll(1);
            consumer.seekToBeginning(consumer.assignment());
            consumer.poll(1);
            consumer.commitSync();
            consumer.close();
            reset = true;
            return senario_number;
        } else if (senario_number == 4) {
            // ???????????? ?????????.
            return senario_number;
        }
        return senario_number;
    }

    public static void chattingWindowPrompt() {
        System.out.println("");
        System.out.println("Chatting Window");
        System.out.println("1. List");
        System.out.println("2. Make");
        System.out.println("3. Join");
        System.out.println("4. Log out");
        System.out.println("");

    }

    public static int chattingWindowInput()
            throws InterruptedException, ExecutionException, FileNotFoundException, IOException {
            int senario_number = 5;
            Scanner senario = new Scanner(System.in);
            System.out.print("SogangTalk> ");
            senario_number = senario.nextInt();

            if (senario_number == 1) {
                Set<String> list = kafkaTopicList();
                for (String s : list) {
                    System.out.println(s);
                }
                return senario_number;
            } else if (senario_number == 2) {
                // topic ??????
                System.out.print("SogangTalk> Chat room name: ");
                Scanner nameString = new Scanner(System.in);
                String room_name = nameString.nextLine();
                Set<String> list = kafkaTopicList();
                boolean flag = false;
                for (String s : list) {
                    if (s.equals(room_name)) {
                        System.out.println("room " + room_name + " already exists");
                        flag = true;
                        break;
                    }
                }
                if (flag == false){
                    System.out.println("\"" + room_name + "\" is created!");
                    kafkaTopicCreate(room_name);
                }
                return senario_number;
            } else if (senario_number == 3) {
                // topic ???????????? ????????? break
                System.out.print("SogangTalk> Chat room name: ");
                Scanner nameString = new Scanner(System.in);
                String topicName = nameString.nextLine();
                Set<String> list = kafkaTopicList();
                int ret = -1;
                for (String s : list) {
                    if (s.equals(topicName)) {
                        topicString = topicName;
                        chattingRoomPrompt(topicString);
                        while (ret != 4) {
                            ret = chattingRoomInput(topicString);
                        }
                    }
                }
                if (ret == -1) {
                    System.out.println("no chatting room named " + topicName);
                }
                return senario_number;

            } else if (senario_number == 4) {
                // ???????????? ?????????.
                return senario_number;
            }
            return senario_number;
    }

    public static void main(String[] args)
            throws InterruptedException, ExecutionException, FileNotFoundException, IOException {
        // producer();
        // consumer();
        // ????????? ?????? producer ??? consumer ?????? ????????? ?????? ???????????? ??????.
        // ?????? id ?????? ??????
        // ?????? ????????? list??? ????????????. producer consumer?
        // ?????? ?????? ??????

        while (true) {
            loginPrompt();
            ID_STRING = loginInput();

            if (ID_STRING.isEmpty()) {
                return;
            }
            int ret = 3;
            while (true) {
                if (ret == 3){
                    chattingWindowPrompt();
                }
                ret = chattingWindowInput();
                if (ret == 4){
                    break;
                }
            }
        }
        // ?????? ??????

    }
}
